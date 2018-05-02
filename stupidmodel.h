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

	int rowCount(const QModelIndex &parent) const { return myData.size() > 0 ? myData[0].size() : 0; }
	int columnCount(const QModelIndex &parent) const { return myData.size(); }

	QVariant data(const QModelIndex &index, int role) const;

	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
	std::vector<std::vector<QString>> myData;
};

#endif // STUPIDMODEL_H
