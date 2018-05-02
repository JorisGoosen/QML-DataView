#ifndef DATASETVIEW_H
#define DATASETVIEW_H

#include <QObject>
#include <QQuickItem>
#include <QAbstractTableModel>
#include <vector>
#include <QSGFlatColorMaterial>

class DataSetView : public QQuickItem
{
	Q_OBJECT
	Q_PROPERTY( QAbstractTableModel * model READ model					WRITE setModel					NOTIFY modelChanged )
	Q_PROPERTY( float fontPixelSize			READ fontPixelSize			WRITE setFontPixelSize			NOTIFY fontPixelSizeChanged )
	Q_PROPERTY( float itemHorizontalPadding READ itemHorizontalPadding	WRITE setItemHorizontalPadding	NOTIFY itemHorizontalPaddingChanged )
	Q_PROPERTY( float itemVerticalPadding	READ itemVerticalPadding	WRITE setItemVerticalPadding	NOTIFY itemVerticalPaddingChanged )

public:
	DataSetView();

	QAbstractTableModel * model() { return _model; }
	void setModel(QAbstractTableModel * model);
	void createDataItems();
	void calculateCellSizes();

	float fontPixelSize()			{ return _fontPixelSize;}
	float itemHorizontalPadding()	{ return _itemHorizontalPadding;}
	float itemVerticalPadding()		{ return _itemVerticalPadding;}

	void setFontPixelSize(float newPixelSize)					{ if(newPixelSize != _fontPixelSize)					{ _fontPixelSize = newPixelSize;					emit fontPixelSizeChanged(); }}
	void setItemHorizontalPadding(float newHorizontalPadding)	{ if(newHorizontalPadding != _itemHorizontalPadding)	{ _itemHorizontalPadding = newHorizontalPadding;	emit itemHorizontalPaddingChanged(); }}
	void setItemVerticalPadding(float newVerticalPadding)		{ if(newVerticalPadding != _itemVerticalPadding)		{ _itemVerticalPadding = newVerticalPadding;		emit itemVerticalPaddingChanged(); }}

protected:
	QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override;

	QAbstractTableModel * _model = NULL;

	std::vector<std::vector<QQuickItem*>> _dataItems;
	std::vector<float> _dataColsMaxWidth, _dataRowsMaxHeight;
	QQmlComponent * itemComp = NULL;
	bool recalculateCellSizes = false;

	float _fontPixelSize = 20;
	float _itemHorizontalPadding = 4;
	float _itemVerticalPadding = 4;

	QSGFlatColorMaterial material;

signals:
	void modelChanged();
	void fontPixelSizeChanged();
	void itemHorizontalPaddingChanged();
	void itemVerticalPaddingChanged();


public slots:
	void aContentSizeChanged() { recalculateCellSizes = true; }
};

#endif // DATASETVIEW_H
