#include "datasetview.h"

#include <QSGFlatColorMaterial>
#include <QSGGeometry>
#include <QSGNode>
#include <iostream>
#include <queue>

DataSetView::DataSetView()
{
	setFlag(QQuickItem::ItemHasContents, true);

	material.setColor(Qt::black);
}

void DataSetView::setModel(QAbstractTableModel * model)
{
	if(_model != model)
	{
		std::cout << "model set!\n" << std::flush;
		_model = model;
		//createDataItems();

		emit modelChanged();
	}
}

void DataSetView::createDataItems()
{
	if(itemComp == NULL)
	{
		itemComp = new QQmlComponent(qmlEngine(this));
		itemComp->setData("import QtQuick 2.10\nText { text: \"???\" }", QUrl());
	}

	if(_dataItems.size() > 0)
		for(auto & col : _dataItems)
			if(col.size() > 0)
				for(auto & row : col)
					delete row;
	_dataItems.clear();
	_dataColsMaxWidth.clear();
	_dataRowsMaxHeight.clear();

	_dataItems.resize(_model->columnCount());

	for(int col=0; col<_model->columnCount(); col++)
	{
		_dataItems[col].resize(_model->rowCount());

		for(int row=0; row<_model->rowCount(); row++)
		{
			QQuickItem * textItem = qobject_cast<QQuickItem*>(itemComp->create());

			textItem->setProperty("text", _model->data(_model->index(row, col)).toString());
			textItem->setParent(this);
			textItem->setParentItem(this);
			textItem->setVisible(true);

			QObject::connect(textItem, SIGNAL(contentSizeChanged()), this, SLOT(aContentSizeChanged()));

			_dataItems[col][row] = textItem;
		}
	}

	calculateCellSizes();

}

void DataSetView::calculateCellSizes()
{
	_dataColsMaxWidth.resize(_model->columnCount());

	for(int col=0; col<_model->columnCount(); col++)
	{
		_dataColsMaxWidth[col] = 0;

		for(int row=0; row<_model->rowCount(); row++)
			_dataColsMaxWidth[col] = std::max(_dataColsMaxWidth[col], _dataItems[col][row]->property("contentWidth").toFloat());
	}

	for(int col=0; col<_model->columnCount(); col++)
		_dataColsMaxWidth[col] += _itemHorizontalPadding * 2;

	_dataRowsMaxHeight.resize(_model->rowCount());

	for(int row=0; row<_model->rowCount(); row++)
	{
		_dataRowsMaxHeight[row] = 0;

		for(int col=0; col<_model->columnCount(); col++)
		{
			_dataRowsMaxHeight[row] = std::max(_dataRowsMaxHeight[row], _dataItems[col][row]->property("contentHeight").toFloat());
		}
	}

	for(int row=0; row<_model->rowCount(); row++)
		_dataRowsMaxHeight[row] += _itemVerticalPadding * 2;


	float w=0, h=0;
	for(int col=0; col<_model->columnCount(); col++)
	{
		float y = _itemVerticalPadding;
		w += _dataColsMaxWidth[col];

		for(int row=0; row<_model->rowCount(); row++)
		{
			_dataItems[col][row]->setY(y);
			y += _dataRowsMaxHeight[row];
		}
	}

	for(int row=0; row<_model->rowCount(); row++)
	{
		float x = _itemHorizontalPadding;
		h += _dataRowsMaxHeight[row];

		for(int col=0; col<_model->columnCount(); col++)
		{
			_dataItems[col][row]->setX(x);
			x += _dataColsMaxWidth[col];

		}
	}

	setWidth(w);
	setHeight(h);
	recalculateCellSizes = false;
}


QSGNode * DataSetView::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
	if (width() <= 0 || height() <= 0) {
		delete oldNode;
		return 0;
	}

	if(recalculateCellSizes) calculateCellSizes();

	const QRectF rect = boundingRect();

	int rows = _model == NULL ? 2 : _model->rowCount();
	int cols = _model == NULL ? 2 : _model->columnCount();

	std::queue<std::pair<float, float>> linePoints;

	float y0 = rect.top();
	for(int r=0; r<rows; r++)
	{
		float x0 = rect.left();
		float dY = _dataRowsMaxHeight[r];

		for(int c=0; c<cols; c++)
		{

			float dX = _dataColsMaxWidth[c];

			linePoints.push(std::make_pair(x0,		y0));
			linePoints.push(std::make_pair(x0,		y0 + dY));

			linePoints.push(std::make_pair(x0,		y0));
			linePoints.push(std::make_pair(x0+ dX,	y0));

			linePoints.push(std::make_pair(x0 + dX,	y0 + dY));
			linePoints.push(std::make_pair(x0,		y0 + dY));

			linePoints.push(std::make_pair(x0 + dX,	y0 + dY));
			linePoints.push(std::make_pair(x0 + dX,	y0));

			x0 += dX;
		}

		y0 += dY;
	}

	const int linesPerNode = 1000; //Or something? should be multiple of 2 though

	if(!oldNode)
	{
		oldNode = new QSGNode();
	}

	//QSGGeometryNode *lines = static_cast<QSGGeometryNode*>(oldNode);

	QSGGeometryNode * currentNode = static_cast<QSGGeometryNode*>(oldNode->firstChild());



	while(linePoints.size() > 0)
	{
		if(currentNode == NULL)
		{
			currentNode = new QSGGeometryNode;

			currentNode->setFlag(QSGNode::OwnsMaterial, false);
			currentNode->setFlag(QSGNode::OwnsGeometry, true);
			currentNode->setMaterial(&material);

			oldNode->appendChildNode(currentNode);
		}

		int geomSize = std::min(linesPerNode, (int)linePoints.size());

		QSGGeometry *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), geomSize);
		geometry->setLineWidth(2);
		geometry->setDrawingMode(GL_LINES);
		QSGGeometry::Point2D *points = geometry->vertexDataAsPoint2D();

		for(int i=geomSize; i>0; i--)
		{
			auto p = linePoints.front();
			linePoints.pop();

			points[geomSize - i].x = p.first;
			points[geomSize - i].y = p.second;
		}

		currentNode->setGeometry(geometry);

		currentNode = static_cast<QSGGeometryNode*>(currentNode->nextSibling());
	}

	return oldNode;
}

/*

QSGNode *DataSetView::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
	if (width() <= 0 || height() <= 0) {
		delete oldNode;
		return 0;
	}

	QSGGeometryNode *triangle = static_cast<QSGGeometryNode*>(oldNode);
	if (!triangle) {
		triangle = new QSGGeometryNode;
		triangle->setFlag(QSGNode::OwnsMaterial, true);
		triangle->setFlag(QSGNode::OwnsGeometry, true);
	}

	const QRectF rect = boundingRect();

	int rows = _model == NULL ? 2 : _model->rowCount();
	int cols = _model == NULL ? 2 : _model->columnCount();

	QSGGeometry *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 2 * rows * cols);
	geometry->setDrawingMode(GL_LINES);
	QSGGeometry::Point2D *points = geometry->vertexDataAsPoint2D();

	float dX = rect.width()	 / rows;
	float dY = rect.height() / cols;

	for(int c=0; c<cols; c++)
		for(int r=0; r<rows; r++)
		{
			int offset	= (c * rows + r) * 2;

			float x0	= r * dX + rect.left();
			float y0	= c * dY + rect.top();

			points[offset].x = x0;
			points[offset].y = y0;

			points[offset + 1].x = x0  + dX;
			points[offset + 1].y = y0 + dY;

			//points[offset + 2].x = x0 + dX;
			//points[offset + 2].y = y0;
		}

	triangle->setGeometry(geometry);

	QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
	material->setColor(Qt::blue);
	triangle->setMaterial(material);

	return triangle;
}

*/
