/*
* ============================================================================
*  Name         : proxywidget.cpp
*  Part of      : MoGateway
*  Description  : Proxy for QML
*  Author     	: Leon Anavi
*  Email		: leon@anavi.org
*  License      : GNU General Public License version 3 (GPLv3)
*
*  Copyright (c) 2012
* ============================================================================
*/

#include "proxywidget.h"

#include <QLabel>

#include "mainwindow.h"

#include <QGraphicsProxyWidget>
#include <QApplication>
#include <QDesktopWidget>

ProxyWidget::ProxyWidget(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    mEmbeddedWidget(new MainWindow())
{
    QRect Screen = QApplication::desktop()->screenGeometry();

    mProxyWidget = new QGraphicsProxyWidget(this);
    mProxyWidget->setWidget(mEmbeddedWidget);
    //mEmbeddedWidget->setGeometry(0,0, Screen.width(), Screen.height());
    //mProxyWidget->setGeometry( QRect(0,0, Screen.width(), Screen.height()) );
    mEmbeddedWidget->setAttribute(Qt::WA_TranslucentBackground);
}
//------------------------------------------------------------------------------

void ProxyWidget::geometryChanged(const QRectF &newGeometry, const QRectF & oldGeometry)
{
    Q_UNUSED(oldGeometry)
    mEmbeddedWidget->resizeGui(newGeometry.width(), newGeometry.height());
    mProxyWidget->setGeometry(newGeometry);
}
//------------------------------------------------------------------------------
