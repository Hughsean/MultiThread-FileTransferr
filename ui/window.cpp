//
// Created by xSeung on 2023/4/26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_window.h" resolved

#include "window.h"
#include "ui_window.h"

namespace cncd {
        window::window(QWidget *parent) : QWidget(parent), ui(new Ui::window) {
                ui->setupUi(this);
        }

        window::~window() {
                delete ui;
        }
}  // namespace cncd
