//
// Created by xSeung on 2023/4/26.
//

#ifndef CN_CD_WINDOW_H
#define CN_CD_WINDOW_H

#include <QWidget>

namespace cncd {
        QT_BEGIN_NAMESPACE
        namespace Ui {
                class window;
        }
        QT_END_NAMESPACE

        class window : public QWidget {
                Q_OBJECT

            public:
                explicit window(QWidget *parent = nullptr);
                ~window() override;

            private:
                Ui::window *ui;
        };
}  // namespace cncd

#endif  // CN_CD_WINDOW_H
