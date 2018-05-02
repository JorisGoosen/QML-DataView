#include "stupidmodel.h"

StupidModel::StupidModel()
{
	int colSize = 1000, rowSize = 1000;
	myData.resize(colSize);

	for(int col=0; col<colSize; col++)
	{
		myData[col].resize(rowSize);

		for(int row=0; row<rowSize; row++)
			myData[col][row] = QString::fromStdString("Cell(" + std::to_string(col) + ", " + std::to_string(row) + ")");
	}

}


QVariant  StupidModel::data(const QModelIndex &index, int role) const
{
	if(role == Qt::DisplayRole && index.column()  < myData.size() && index.row() <	myData[index.column()].size())
		return QVariant(myData[index.column()][index.row()]);
	return QVariant();
}

QVariant StupidModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation == Qt::Horizontal)
	{
		if(role == Qt::DisplayRole && section < myData.size())
			return QVariant(QString::fromStdString("Column " + std::to_string(section)));
	}
	else if(myData.size() > 0)
	{
		if(role == Qt::DisplayRole && section < myData[0].size())
			return QVariant(QString::fromStdString("Row " + std::to_string(section)));
	}

	return QVariant();
}
