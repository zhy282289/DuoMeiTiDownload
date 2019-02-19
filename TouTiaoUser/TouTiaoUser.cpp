#include "stdafx.h"
#include "TouTiaoUser.h"

TouTiaoUser::TouTiaoUser(QWidget *parent)
	: QWidget(parent)
{
	resize(800, 400);

	m_unFollowView = nullptr;
	m_btnUnFollow = new QPushButton(TR("·´¹Ø×¢"), this);

	connect(m_btnUnFollow, &QPushButton::clicked, this, [=]() {
		if (m_unFollowView)
			delete m_unFollowView;
		//if (m_unFollowView == nullptr)
		{
			m_unFollowView = new HtmlView(0);
			m_unFollowView->resize(800,800);
			m_unFollowView->show();
			//m_unFollowView->load(QUrl("https://www.toutiao.com/c/user/relation/51620417289/?tab=following#mid=1568287264229378"));
			m_unFollowView->load(QUrl("https://www.baidu.com"));
		}
	});


}

void TouTiaoUser::resizeEvent(QResizeEvent *event)
{
	int left = 20;
	int top = 20;

	const int btnw = 70;
	const int btnh = 22;
	m_btnUnFollow->setGeometry(left, top, btnw, btnh);
}
