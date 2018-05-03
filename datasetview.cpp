#include "datasetview.h"

#include <QSGFlatColorMaterial>
#include <QSGGeometry>
#include <QSGNode>
#include <queue>



DataSetView::DataSetView() : _metricsFont(_font)
{
	setFlag(QQuickItem::ItemHasContents, true);

	material.setColor(Qt::black);

	connect(this, &DataSetView::parentChanged, this, &DataSetView::myParentChanged);

	connect(this, &DataSetView::viewportXChanged, this, &DataSetView::viewportChanged);
	connect(this, &DataSetView::viewportYChanged, this, &DataSetView::viewportChanged);
	connect(this, &DataSetView::viewportWChanged, this, &DataSetView::viewportChanged);
	connect(this, &DataSetView::viewportHChanged, this, &DataSetView::viewportChanged);

	setProperty("ikBestaNiet", "Nu wel!");

}

DataSetView::~DataSetView()
{
	if(itemComp != NULL)
		delete itemComp;
	itemComp = NULL;

	//everything else is cleaned up through QObject tree I think.
}

void DataSetView::setModel(QAbstractTableModel * model)
{
	if(_model != model)
	{
		std::cout << "model set!\n" << std::flush;
		_model = model;

		setRolenames();

		QSizeF calcedSizeRowNumber = _metricsFont.size(Qt::TextSingleLine, QString::fromStdString(std::to_string(_model->rowCount())));
		_rowNumberMaxWidth = calcedSizeRowNumber.width() + 20;

		//recalculateCellSizes = true;
		calculateCellContentSizes();

		emit modelChanged();
	}
}

void DataSetView::setRolenames()
{
	_roleNameToRole.clear();

	if(_model == NULL) return;

	auto roleNames = _model->roleNames();

	for(auto rn : roleNames.keys())
		_roleNameToRole[roleNames[rn].toStdString()] = rn;

}

void DataSetView::calculateCellContentSizes()
{
	/**/

	_cellSizes.clear();
	_dataColsMaxWidth.clear();

	for(auto col : _cellTextItems)
		for(auto row : col.second)
			storeTextItem(row.first, col.first);

	_cellSizes.resize(_model->columnCount());
	_colXPositions.resize(_model->columnCount());
	_cellTextItems.clear();

	for(int col=0; col<_model->columnCount(); col++)
	{
		QString text = _model->headerData(col, Qt::Orientation::Horizontal, _roleNameToRole["maxColString"]).toString();
		QSizeF calcedSize = _metricsFont.size(Qt::TextSingleLine, text);

		_cellSizes[col] = calcedSize;
	}

	calculateCellSizes();

}


void DataSetView::calculateCellSizes()
{

	_dataColsMaxWidth.resize(_model->columnCount());

	for(int col=0; col<_model->columnCount(); col++)
		_dataColsMaxWidth[col] = _cellSizes[col].width() + _itemHorizontalPadding * 2;

	_dataRowsMaxHeight = _model->columnCount() == 0 ? 0 : _cellSizes[0].height() + _itemVerticalPadding * 2;

	float w=_rowNumberMaxWidth;
	for(int col=0; col<_model->columnCount(); col++)
		w += _dataColsMaxWidth[col];


	float x = _itemHorizontalPadding + _rowNumberMaxWidth;

	for(int col=0; col<_model->columnCount(); col++)
	{
		_colXPositions[col] = x;
		x += _dataColsMaxWidth[col];
	}


	setWidth(w);
	setHeight( _dataRowsMaxHeight * _model->rowCount());
	_recalculateCellSizes = false;
}


void DataSetView::viewportChanged()
{
	if(_viewportX != _viewportX || _viewportY != _viewportY || _viewportW != _viewportW || _viewportH != _viewportH ) //only possible if they are NaN
		return;

#ifdef DEBUG_VIEWPORT
	std::cout << "viewportChanged!\n" <<std::flush;
#endif
	QVector2D leftTop(_viewportX * width(), _viewportY * height());
	QVector2D viewSize(_viewportW * width(), _viewportH * height());
	QVector2D rightBottom(leftTop + viewSize);

	int currentViewportColMin = -1, currentViewportColMax = -1, currentViewportRowMin = -1, currentViewportRowMax = -1;

	float cumulative = 0;
	for(int col=0; col<_model->columnCount() && currentViewportColMax == -1; col++)
	{
		if(currentViewportColMax == -1 && cumulative > rightBottom.x())
			currentViewportColMax = col;

		cumulative += _dataColsMaxWidth[col];

		if(currentViewportColMin == -1 && cumulative > leftTop.x())
			currentViewportColMin = col;
	}

	if(currentViewportColMax == -1)
		currentViewportColMax = _model->columnCount();

	currentViewportColMin = std::max(0,						currentViewportColMin - _viewportMargin);
	currentViewportColMax = std::min(_model->columnCount(),	currentViewportColMax + _viewportMargin);

	currentViewportRowMin = std::max(qRound(leftTop.y()		/ _dataRowsMaxHeight) - 1,	0);
	currentViewportRowMax = std::min(qRound(rightBottom.y()	/ _dataRowsMaxHeight) + 1,	_model->rowCount());

	// remove superflouous textItems if they exist (aka store them in stack)
	if(_previousViewportRowMin != -1 && _previousViewportRowMax != -1 && _previousViewportColMin != -1 && _previousViewportColMax != -1)
	{
		for(int col=_previousViewportColMin; col<_previousViewportColMax; col++)
		{
			for(int row=_previousViewportRowMin; row < currentViewportRowMin; row++)
				storeTextItem(row, col);

			for(int row=currentViewportRowMax; row < _previousViewportRowMax; row++)
				storeTextItem(row, col);
		}

		for(int row=_previousViewportRowMin; row<_previousViewportRowMax; row++)
		{
			for(int col=_previousViewportColMin; col < currentViewportColMin; col++)
				storeTextItem(row, col);

			for(int col=currentViewportColMax; col < _previousViewportColMax; col++)
				storeTextItem(row, col);
		}

		for(int row=_previousViewportRowMin; row < currentViewportRowMin; row++)
			storeRowNumber(row);

		for(int row=currentViewportRowMax; row < _previousViewportRowMax; row++)
			storeRowNumber(row);
	}

	_lines.clear();

	//and now we should create some new ones!

	for(int col=currentViewportColMin; col<currentViewportColMax; col++)
		for(int row=currentViewportRowMin; row<currentViewportRowMax; row++)
		{
			int lineFlags = _model->data(_model->index(row, col), _roleNameToRole["lines"]).toInt();

			bool left = lineFlags & 1 > 0,  right = lineFlags & 2 > 0, up = lineFlags & 4 > 0, down = lineFlags & 8 > 0;

			createTextItem(row, col);
			QVector2D pos(_colXPositions[col] - _itemHorizontalPadding, row * _dataRowsMaxHeight);



			if(left)	_lines.push_back(std::make_pair(QVector2D(pos.x(),								pos.y() + _dataRowsMaxHeight),	pos));
			if(up)		_lines.push_back(std::make_pair(QVector2D(pos.x() + _dataColsMaxWidth[col],		pos.y()),						pos));
			if(right)	_lines.push_back(std::make_pair(QVector2D(pos.x(),							pos.y() + _dataRowsMaxHeight),	QVector2D(pos.x() + _dataColsMaxWidth[col], pos.y() + _dataRowsMaxHeight)));
			if(down)	_lines.push_back(std::make_pair(QVector2D(pos.x() + _dataColsMaxWidth[col], pos.y()) ,						QVector2D(pos.x() + _dataColsMaxWidth[col], pos.y() + _dataRowsMaxHeight)));

		}

	for(int row=currentViewportRowMin; row<currentViewportRowMax; row++)
		createRowNumber(row);

	update();

#ifdef DEBUG_VIEWPORT
	std::cout << "viewport X: " << _viewportX << " Y: " << _viewportY << " W: " << _viewportW << " H: " << _viewportH <<  std::endl << std::flush;
	std::cout << "_previousViewport ColMin: "<<_previousViewportColMin<<" ColMax: "<<_previousViewportColMax<<" RowMin: "<<_previousViewportRowMin<<" RowMax: "<<_previousViewportRowMax<<"\n";
	std::cout << "currentViewport ColMin: "<<currentViewportColMin<<" ColMax: "<<currentViewportColMax<<" RowMin: "<<currentViewportRowMin<<" RowMax: "<<currentViewportRowMax<<"\n"<<std::flush;
#endif

	_previousViewportColMin = currentViewportColMin;
	_previousViewportColMax = currentViewportColMax;
	_previousViewportRowMin = currentViewportRowMin;
	_previousViewportRowMax = currentViewportRowMax;
}

QQuickItem * DataSetView::createTextItem(int row, int col)
{
	//std::cout << "createTextItem("<<row<<", "<<col<<") called!\n" << std::flush;

	if((_cellTextItems.count(col) == 0 && _cellTextItems[col].count(row) == 0) || _cellTextItems[col][row] == NULL)
	{

		if(itemComp == NULL)
		{
			itemComp = new QQmlComponent(qmlEngine(this));
			itemComp->setData("import QtQuick 2.10\nText { text: \"???\"; z: 2 }", QUrl());
		}

		QQuickItem * textItem = NULL;

		if(_textItemStorage.size() > 0)
		{
#ifdef DEBUG_VIEWPORT
			std::cout << "createTextItem("<<row<<", "<<col<<") from storage!\n" << std::flush;
#endif
			textItem = _textItemStorage.top();
			_textItemStorage.pop();
		}
		else
		{
#ifdef DEBUG_VIEWPORT
			std::cout << "createTextItem("<<row<<", "<<col<<") ex nihilo!\n" << std::flush;
#endif
			textItem = qobject_cast<QQuickItem*>(itemComp->create());
			textItem->setParent(this);
			textItem->setParentItem(this);
		}

		QModelIndex ind(_model->index(row, col));
		bool active = _model->data(ind, _roleNameToRole["active"]).toBool();
		textItem->setProperty("color", active ? "black" : "grey");
		textItem->setProperty("text", _model->data(ind));
		textItem->setX(_colXPositions[col]);
		textItem->setY(_itemVerticalPadding + row * _dataRowsMaxHeight);
		textItem->setVisible(true);

		_cellTextItems[col][row] = textItem;
	}

	return _cellTextItems[col][row];
}

void DataSetView::storeTextItem(int row, int col)
{
#ifdef DEBUG_VIEWPORT
	std::cout << "storeTextItem("<<row<<", "<<col<<") in storage!\n" << std::flush;
#endif
	if((_cellTextItems.count(col) == 0 && _cellTextItems[col].count(row) == 0) || _cellTextItems[col][row] == NULL) return;



	QQuickItem * textItem = _cellTextItems[col][row];
	_cellTextItems[col][row] = NULL;

	_cellTextItems[col].erase(row);

	if(_cellTextItems[col].size() == 0)
		_cellTextItems.erase(col);

	textItem->setVisible(false);

	_textItemStorage.push(textItem);
}



QQuickItem * DataSetView::createRowNumber(int row)
{
	//std::cout << "createRowNumber("<<row<<") called!\n" << std::flush;

	if(_rowNumberDelegate == NULL)
		return NULL;

	QQuickItem * rowNumber = NULL;

	if(_rowNumberItems.count(row) == 0  || _rowNumberItems[row] == NULL)
	{

		if(_rowNumberStorage.size() > 0)
		{
#ifdef DEBUG_VIEWPORT
			std::cout << "createRowNumber("<<row<<") from storage!\n" << std::flush;
#endif
			rowNumber = _rowNumberStorage.top();
			_rowNumberStorage.pop();
		}
		else
		{
#ifdef DEBUG_VIEWPORT
			std::cout << "createRowNumber("<<row<<") ex nihilo!\n" << std::flush;
#endif
			rowNumber = qobject_cast<QQuickItem*>(_rowNumberDelegate->create());
			rowNumber->setParent(this);
			rowNumber->setParentItem(this);
		}

		rowNumber->setProperty("z", 10);
		rowNumber->setProperty("text", QString::fromStdString(std::to_string(row)));

		rowNumber->setY(_dataRowsMaxHeight * row);
		rowNumber->setHeight(_dataRowsMaxHeight);
		rowNumber->setWidth(_rowNumberMaxWidth);

		rowNumber->setVisible(true);

		_rowNumberItems[row] = rowNumber;
	}
	else
		rowNumber = _rowNumberItems[row];

	rowNumber->setX(_viewportX * width());

	return _rowNumberItems[row];
}

void DataSetView::storeRowNumber(int row)
{
#ifdef DEBUG_VIEWPORT
	std::cout << "storeTextItem("<<row<<", "<<col<<") in storage!\n" << std::flush;
#endif

	if(_rowNumberItems.count(row) == 0  || _rowNumberItems[row] == NULL) return;

	QQuickItem * rowNumber = _rowNumberItems[row];
	_rowNumberItems[row] = NULL;

	_rowNumberItems.erase(row);

	rowNumber->setVisible(false);

	_rowNumberStorage.push(rowNumber);
}

void DataSetView::myParentChanged(QQuickItem * newParentItem)
{

	/*if(newParentItem->property("viewport").isValid())
	{
		QQuickItem * viewport = newParentItem->property("viewport").
		connect
	}
	void xChanged();
	void yChanged();
	void widthChanged();
	void heightChanged();
*/
}


QSGNode * DataSetView::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
	if (width() <= 0 || height() <= 0) {
		delete oldNode;
		return 0;
	}

	//if(recalculateCellSizes) calculateCellContentSizes();

	const QRectF rect = boundingRect();


	const int linesPerNode = 1000; //Or something? should be multiple of 2 though

	if(!oldNode)
	{
		oldNode = new QSGNode();
	}

	//QSGGeometryNode *lines = static_cast<QSGGeometryNode*>(oldNode);

	QSGGeometryNode * currentNode = static_cast<QSGGeometryNode*>(oldNode->firstChild());


	for(int lineIndex=0; lineIndex<_lines.size();)
	{
		if(currentNode == NULL)
		{
			currentNode = new QSGGeometryNode;

			currentNode->setFlag(QSGNode::OwnsMaterial, false);
			currentNode->setFlag(QSGNode::OwnsGeometry, true);
			currentNode->setMaterial(&material);

			oldNode->appendChildNode(currentNode);
		}

		int geomSize = std::min(linesPerNode, (int)(_lines.size() - lineIndex));
		geomSize *= 2;

		QSGGeometry *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), geomSize);
		geometry->setLineWidth(1);
		geometry->setDrawingMode(GL_LINES);
		QSGGeometry::Point2D *points = geometry->vertexDataAsPoint2D();

		for(int geomIndex=0; geomIndex<geomSize; geomIndex+=2)
		{
			points[geomIndex  ].x = _lines[lineIndex].first.x() + rect.left();
			points[geomIndex  ].y = _lines[lineIndex].first.y() + rect.top();
			points[geomIndex+1].x = _lines[lineIndex].second.x() + rect.left();
			points[geomIndex+1].y = _lines[lineIndex].second.y() + rect.top();
			lineIndex++;
		}

		currentNode->setGeometry(geometry);

		currentNode = static_cast<QSGGeometryNode*>(currentNode->nextSibling());
	}


	std::queue<QSGGeometryNode*> killThem;

	while(currentNode != NULL) //superfluous children! Lets kill em
	{
		killThem.push(currentNode);
		currentNode = static_cast<QSGGeometryNode*>(currentNode->nextSibling());
	}

	while(killThem.size() > 0)
	{
		QSGGeometryNode * childToDie = killThem.front();
		killThem.pop();

		delete childToDie;
	}


	return oldNode;
}



void DataSetView::setViewportX(float newViewportX)
{
	if(newViewportX != _viewportX)
	{
		//std::cout << "setViewPortX!\n" <<std::flush;
		_viewportX = newViewportX;
		emit viewportXChanged();
	}
}

void DataSetView::setViewportY(float newViewportY)
{
	if(newViewportY != _viewportY)
	{
		//std::cout << "setViewPortY!\n" << std::flush;
		_viewportY = newViewportY;
		emit viewportYChanged();
	}
}

void DataSetView::setViewportW(float newViewportW)
{
	if(newViewportW != _viewportW)
	{
		//std::cout << "setViewPortW!\n" << std::flush;
		_viewportW = newViewportW;
		emit viewportWChanged();
	}
}

void DataSetView::setViewportH(float newViewportH)
{
	if(newViewportH != _viewportH)
	{
		//std::cout << "setViewPortH!\n" << std::flush;
		_viewportH = newViewportH;
		emit viewportHChanged();
	}
}
