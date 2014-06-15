#ifndef INITIALCONDFACTORY_H
#define INITIALCONDFACTORY_H

#include "headers.hpp"
#include "initialCond.hpp"

class InitialCondFactory : public QObject {
	Q_OBJECT

    public:
        InitialCond<float> *getInitialCond(int i);
        static const QString getInitialCondName(int i);

	private:
		QString _code;
		bool _codeSent;

	signals:
		bool showCodeEditor(int);

	public slots:
		void codeSubmitted(const QString &);
};

#endif

