#include "stupidmodel.h"

StupidModel::StupidModel()
{

	/*myData.resize(colSize);

	for(int col=0; col<colSize; col++)
	{
		myData[col].resize(rowSize);

		for(int row=0; row<rowSize; row++)
			myData[col][row] = QString::fromStdString("Cell(" + std::to_string(col) + ", " + std::to_string(row) + ")");
	}
*/
}


QVariant  StupidModel::data(const QModelIndex &index, int role) const
{
	if(index.column()  < columnCount() && index.row() <	rowCount())
	{
		bool iAmActive = index.row() % 7 < 3;

		if(role == Qt::DisplayRole)
			return QVariant(QString::fromStdString("Cell(" + std::to_string(index.column()) + ", " + std::to_string(index.row()) + ")")+ getExtraShit(index.column()));//myData[index.column()][index.row()]);
		else if(role == (int)specialRoles::active)
			return iAmActive;
		else if(role == (int)specialRoles::lines)
		{
			bool left = iAmActive, right = iAmActive && index.column() == columnCount() - 1; //always draw left line and right line only if last col
			bool up = iAmActive || (index.row() > 0 && data(this->index(index.row() - 1, index.column()), (int)specialRoles::active).toBool()); //draw upper line if i am active or if not when my upstairs neighbour is active
			bool down = (iAmActive && index.row() == rowCount() - 1) || (!iAmActive && index.row() < rowCount() - 1 && data(this->index(index.row() + 1, index.column()), (int)specialRoles::active).toBool()); //draw down line only if i am the last row or if i am inactive and my downstairs neighbour is active

			return	(left ?		1 : 0) +
					(right ?	2 : 0) +
					(up ?		4 : 0) +
					(down ?		8 : 0);
		}
	}

	return QVariant();
}

QVariant StupidModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation == Qt::Horizontal && section < columnCount())
	{
		if(role == Qt::DisplayRole)
			return QVariant(QString::fromStdString("Column " + std::to_string(section)));
		else if(role == (int)specialRoles::maxColString) //A query from DataSetView for the maximumlength string to be expected! This to accomodate columnwidth
			return QVariant(QString::fromStdString("Cell("+std::to_string(section) + ", " + std::to_string(rowCount()) + ")" )+ getExtraShit(section)); //In this case this means the last element because that has the highest rownumber!
	}
	else if(section < rowCount())
	{
		if(role == Qt::DisplayRole)
			return QVariant(QString::fromStdString("Row " + std::to_string(section)));
	}

	return QVariant();
}


QHash<int, QByteArray> StupidModel::roleNames() const
{
	QHash<int, QByteArray> roles = QAbstractTableModel::roleNames ();

	roles[(int)specialRoles::active]		= QString("active").toUtf8();
	roles[(int)specialRoles::lines]			= QString("lines").toUtf8();
	roles[(int)specialRoles::maxColString]	= QString("maxColString").toUtf8();

	return roles;
}

void StupidModel::randomShit()
{
	int randomNumber = rand()%columnCount();

	extraShit[randomNumber] = extraShit[randomNumber] + "?";

	emit dataChanged(index(randomNumber, 1), index(randomNumber, columnCount()));
}

