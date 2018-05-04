#ifndef STUPIDMODEL_H
#define STUPIDMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <vector>
#include <QString>

class StupidModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	StupidModel();

	enum class specialRoles { active = Qt::UserRole, lines, maxColString };

	int rowCount(const QModelIndex &parent = QModelIndex()) const { return rowSize; }//myData.size() > 0 ? myData[0].size() : 0; }
	int columnCount(const QModelIndex &parent = QModelIndex()) const { return colSize; } //myData.size(); }

	QVariant data(const QModelIndex &index, int role) const;

	QVariant headerData(int section, Qt::Orientation orientation, int role) const;


	QHash<int, QByteArray> roleNames() const;

	Q_INVOKABLE void resetMe() { beginResetModel(); endResetModel(); }
	Q_INVOKABLE void randomShit();

	QString getExtraShit(int col) const { return extraShit.count(col) > 0 ?  extraShit.at(col) : ""; }


private:
	//std::vector<std::vector<QString>> myData;
	int colSize = 10, rowSize = 10000;

	std::map<int, QString> extraShit;
};

#endif // STUPIDMODEL_H
