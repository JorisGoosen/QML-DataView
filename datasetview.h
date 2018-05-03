#ifndef DATASETVIEW_H
#define DATASETVIEW_H

#include <QObject>
#include <QQuickItem>
#include <QAbstractTableModel>
#include <vector>
#include <stack>
#include <QSGFlatColorMaterial>
#include <iostream>
#include <map>

//#define DEBUG_VIEWPORT

class DataSetView : public QQuickItem
{
	Q_OBJECT
	Q_PROPERTY( QAbstractTableModel * model READ model					WRITE setModel					NOTIFY modelChanged )
	Q_PROPERTY( float fontPixelSize			READ fontPixelSize			WRITE setFontPixelSize			NOTIFY fontPixelSizeChanged )
	Q_PROPERTY( float itemHorizontalPadding READ itemHorizontalPadding	WRITE setItemHorizontalPadding	NOTIFY itemHorizontalPaddingChanged )
	Q_PROPERTY( float itemVerticalPadding	READ itemVerticalPadding	WRITE setItemVerticalPadding	NOTIFY itemVerticalPaddingChanged )

	Q_PROPERTY( float viewportX				READ viewportX				WRITE setViewportX				NOTIFY viewportXChanged )
	Q_PROPERTY( float viewportY				READ viewportY				WRITE setViewportY				NOTIFY viewportYChanged )
	Q_PROPERTY( float viewportW				READ viewportW				WRITE setViewportW				NOTIFY viewportWChanged )
	Q_PROPERTY( float viewportH				READ viewportH				WRITE setViewportH				NOTIFY viewportHChanged )

	Q_PROPERTY( QFont font	MEMBER _font)

public:
	DataSetView();
	~DataSetView();

	QAbstractTableModel * model() { return _model; }
	void setModel(QAbstractTableModel * model);
	void calculateCellContentSizes();
	void calculateCellSizes();

	float fontPixelSize()			{ return _fontPixelSize;}
	float itemHorizontalPadding()	{ return _itemHorizontalPadding;}
	float itemVerticalPadding()		{ return _itemVerticalPadding;}

	float viewportX()				{ return _viewportX; }
	float viewportY()				{ return _viewportY; }
	float viewportW()				{ return _viewportW; }
	float viewportH()				{ return _viewportH; }

	void setViewportX(float newViewportX);
	void setViewportY(float newViewportY);
	void setViewportW(float newViewportW);
	void setViewportH(float newViewportH);

	void setFontPixelSize(float newPixelSize)					{ if(newPixelSize != _fontPixelSize)					{ _fontPixelSize = newPixelSize;					emit fontPixelSizeChanged(); }}
	void setItemHorizontalPadding(float newHorizontalPadding)	{ if(newHorizontalPadding != _itemHorizontalPadding)	{ _itemHorizontalPadding = newHorizontalPadding;	emit itemHorizontalPaddingChanged(); }}
	void setItemVerticalPadding(float newVerticalPadding)		{ if(newVerticalPadding != _itemVerticalPadding)		{ _itemVerticalPadding = newVerticalPadding;		emit itemVerticalPaddingChanged(); }}

protected:
	QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override;

	QAbstractTableModel * _model = NULL;

	std::vector<QSizeF> _cellSizes; //[col]
	std::vector<std::vector<QVector2D>>  _cellPositions; //[col][row]
	std::map<int, std::map<int, QQuickItem *>> _cellTextItems;			//[col][row]
	std::stack<QQuickItem*> _textItemStorage;
	std::vector<float> _dataColsMaxWidth;
	std::vector<std::pair<QVector2D, QVector2D>> _lines;

	float _dataRowsMaxHeight;
	QQmlComponent * itemComp = NULL;
	bool _recalculateCellSizes = false, _ignoreViewpoint = true;

	float _fontPixelSize = 20;
	float _itemHorizontalPadding = 8;
	float _itemVerticalPadding = 8;

	QSGFlatColorMaterial material;

	QFont _font;

	float _viewportX=0, _viewportY=0, _viewportW=1, _viewportH=1;
	int _previousViewportColMin = -1, _previousViewportColMax = -1, _previousViewportRowMin = -1, _previousViewportRowMax = -1, _viewportMargin = 10;

	QQuickItem * createTextItem(int row, int col);
	void storeTextItem(int row, int col);


	std::map<std::string, int> _roleNameToRole;

	void setRolenames();

signals:
	void modelChanged();
	void fontPixelSizeChanged();
	void itemHorizontalPaddingChanged();
	void itemVerticalPaddingChanged();

	void viewportXChanged();
	void viewportYChanged();
	void viewportWChanged();
	void viewportHChanged();



public slots:
	void aContentSizeChanged() { _recalculateCellSizes = true; }
	void viewportChanged();
	void myParentChanged(QQuickItem *);

};

#endif // DATASETVIEW_H
