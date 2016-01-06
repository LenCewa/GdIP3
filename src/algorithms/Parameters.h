#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <QObject>

class Parameters : public QObject
{
Q_OBJECT

public:
   Parameters();

   bool   param1;
   int    param2;
   double param3;
   int paramL;
   int kernelSize;
   int sig_r;
   int sig_d;
   int tau;


public slots:
   void setParam1(bool value)              { param1 = value;            }
   void setParam2(const QString &value)    { param2 = value.toInt();    }
   void setParam3(const QString &value)    { param3 = value.toDouble(); }
   void setParamL(const QString &value)    { paramL = value.toInt();	}
   void setKernelSize(const QString &value){ kernelSize = value.toInt();}
   void setSig_r(const QString &value)     { sig_r = value.toInt();		}
   void setSig_d(const QString &value)     { sig_d = value.toInt();		}
   void setTau(const QString &value)	   { tau = value.toInt();		}
};

#endif
