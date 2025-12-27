#include "gui.h"

#include "conversion.h"

#include <QApplication>
#include <QFont>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegExpValidator>
#include <QString>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>
#include <string>
#include <vector>

#ifdef QT_STATIC_BUILD
#include <QtPlugin>
# ifdef WIN32
  Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
# endif
#endif

#ifndef INPUT_MAX_CHARS
#define INPUT_MAX_CHARS 100
#endif

#ifndef SUMMARY_TEXT_INTEGER_BITS_MAX_CHARS
#define SUMMARY_TEXT_INTEGER_BITS_MAX_CHARS 90
#endif

namespace gui {

using namespace std;
using namespace conversion;

namespace {

QString BuildGroupedBitLines(const string &bits, size_t bitsPerLine = 32) {
  QStringList lines;
  for (size_t i = 0; i < bits.size(); i += bitsPerLine) {
    string chunk = bits.substr(i, bitsPerLine);
    string grouped;
    for (size_t j = 0; j < chunk.size(); ++j) {
      if (j > 0 && j % 4 == 0) {
        grouped.push_back(' ');
      }
      grouped.push_back(chunk[j]);
    }
    lines.push_back(QString::fromStdString(grouped));
  }
  return lines.join("\n");
}

QString TruncateWithDots(const QString& s, int maxLength) {
    if (s.length() <= maxLength) {
        return s; // No need to truncate
    }
    return s.left(maxLength - 3) + "...";
}

QString BuildStripLabel(const ConversionResult &result) {
  QString sign = QString("Znak (1): %1").arg(
      QString::fromStdString(result.signBit));
  QString exp = QString("Wykładnik (15): %1").arg(
      QString::fromStdString(result.exponentBits));
  QString mantissa = QString("Mantysa (112): w panelu poniżej");
  return QStringList{sign, exp, mantissa}.join("\n");
}

class ConverterWindow : public QMainWindow {
public:
  ConverterWindow();

private:
  void HandleConvert();
  void HandleClear();
  void RenderResult(const ConversionResult &result);
  void RenderPlaceholder();
  void SetStatus(const QString &text, const QString &color);
  void HandleInputRejected();
  void HandleInputTextEdited();

  QLineEdit *input_ = nullptr;
  QLabel *status_ = nullptr;
  QLabel *summary_ = nullptr;
  QLabel *strip_ = nullptr;
  QPlainTextEdit *mantissa_ = nullptr;
  QPlainTextEdit *bits_ = nullptr;
};

ConverterWindow::ConverterWindow() {
  setWindowTitle("Konwerter IEEE 754 (128 bitów)");
  setMinimumSize(1000, 640);

  auto *central = new QWidget(this);
  central->setStyleSheet(
      "background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
      " stop:0 #0D1628, stop:1 #0A0F1A);");
  auto *layout = new QVBoxLayout(central);
  layout->setContentsMargins(28, 24, 28, 24);
  layout->setSpacing(14);

  QFont monoFont("Monospace");
  monoFont.setStyleHint(QFont::TypeWriter);

  auto *title = new QLabel("Konwerter IEEE 754 (128 bitów)", this);
    title->setStyleSheet(
      "font-size: 26px; font-weight: 700; letter-spacing: 0.5px;"
      " color: #E8ECF3;");
  auto *subtitle = new QLabel("Interfejs Qt z konwersją na żywo", this);
    subtitle->setStyleSheet(
      "font-size: 14px; color: #B8C1D1; letter-spacing: 0.3px;");
  layout->addWidget(title);
  layout->addWidget(subtitle);

  input_ = new QLineEdit(this);
  input_->setPlaceholderText("Wpisz liczbę dziesiętną (np. 263.3)");
  input_->setClearButtonEnabled(true);
  input_->setMinimumHeight(42);
  input_->setStyleSheet(R"(
      QLineEdit {
        background: #0F1626;
        color: #E8ECF3;
        border: 1px solid #2E3D57;
        padding: 12px;
        border-radius: 10px;
        font-size: 16px;
      }
      QLineEdit:focus {
        border: 1px solid #5A8DFF;
      }
  )");
  input_->setMaxLength(INPUT_MAX_CHARS);
  QRegExp inputValidatorRegExp = QRegExp("^(-?)([0-9]*)([,.]?)([0-9]*)$");
  QRegExpValidator *inputValidator = new QRegExpValidator(inputValidatorRegExp, this);
  input_->setValidator(inputValidator);
  layout->addWidget(input_);

  auto *buttonRow = new QHBoxLayout();
  buttonRow->setSpacing(12);
  buttonRow->setContentsMargins(0, 0, 0, 2);

  auto *convertBtn = new QPushButton("Konwertuj", this);
    convertBtn->setStyleSheet(R"(
      QPushButton { background: #466CBE; color: #F4F7FB; border: none;
            padding: 10px 16px; border-radius: 8px; font-size: 15px; }
      QPushButton:hover { background: #567FD9; }
      QPushButton:pressed { background: #3A5AA0; }
    )");
  auto *clearBtn = new QPushButton("Wyczyść", this);
    clearBtn->setStyleSheet(R"(
      QPushButton { background: #6C7485; color: #F4F7FB; border: none;
            padding: 10px 16px; border-radius: 8px; font-size: 15px; }
      QPushButton:hover { background: #7D869B; }
      QPushButton:pressed { background: #575F70; }
    )");

  buttonRow->addWidget(convertBtn);
  buttonRow->addWidget(clearBtn);
  buttonRow->addStretch();
  layout->addLayout(buttonRow);

  status_ = new QLabel("Wpisz liczbę dziesiętną i kliknij Konwertuj.", this);
  status_->setWordWrap(true);
  status_->setStyleSheet(
      "background: #1C2636; color: #E8ECF3; padding: 12px 14px;"
      "border-radius: 12px; font-size: 14px; letter-spacing: 0.2px;");
  layout->addWidget(status_);

  auto *summaryFrame = new QFrame(this);
  summaryFrame->setFrameShape(QFrame::StyledPanel);
  summaryFrame->setFrameShadow(QFrame::Plain);
  summaryFrame->setStyleSheet(
      "QFrame { background: #0D131F; border: none; border-radius: 10px; }");
  auto *summaryLayout = new QVBoxLayout(summaryFrame);
  summaryLayout->setContentsMargins(16, 14, 16, 14);
  summaryLayout->setSpacing(8);
  summary_ = new QLabel("Wynik pojawi się po konwersji.", summaryFrame);
  summary_->setWordWrap(true);
  summary_->setStyleSheet("color: #E8ECF3; font-size: 15px;");
  summaryLayout->addWidget(summary_);

  strip_ = new QLabel(summaryFrame);
  strip_->setWordWrap(true);
  strip_->setStyleSheet("color: #9AC7FF; font-family: 'Courier New', monospace;"
                        " font-size: 14px; margin-top: 6px;");
  summaryLayout->addWidget(strip_);
  layout->addWidget(summaryFrame);

    auto *mantissaFrame = new QFrame(this);
    mantissaFrame->setFrameShape(QFrame::StyledPanel);
    mantissaFrame->setFrameShadow(QFrame::Plain);
    mantissaFrame->setStyleSheet(
        "QFrame { background: #0E1524; border: none; border-radius: 10px; }");
    auto *mantissaLayout = new QVBoxLayout(mantissaFrame);
      mantissaLayout->setContentsMargins(16, 14, 16, 14);
      mantissaLayout->setSpacing(8);

    auto *mantissaTitle = new QLabel("Mantysa (112 bitów)", mantissaFrame);
    mantissaTitle->setStyleSheet(
      "color: #E8ECF3; font-size: 15px; font-weight: 600;");
    mantissaLayout->addWidget(mantissaTitle);

    mantissa_ = new QPlainTextEdit(mantissaFrame);
    mantissa_->setReadOnly(true);
    mantissa_->setMinimumHeight(140);
    mantissa_->setLineWrapMode(QPlainTextEdit::NoWrap);
    mantissa_->setFont(monoFont);
    mantissa_->setStyleSheet(
        "QPlainTextEdit { background: #0F1626; color: #C9D6EA;"
        " border: none; border-radius: 8px; padding: 10px; }");
    mantissaLayout->addWidget(mantissa_);
    layout->addWidget(mantissaFrame);

  auto *bitsFrame = new QFrame(this);
  bitsFrame->setFrameShape(QFrame::StyledPanel);
      bitsFrame->setFrameShadow(QFrame::Plain);
  bitsFrame->setStyleSheet(
      "QFrame { background: #0A0F1A; border: none; border-radius: 10px; }");
  auto *bitsLayout = new QVBoxLayout(bitsFrame);
  bitsLayout->setContentsMargins(16, 14, 16, 14);
  bitsLayout->setSpacing(8);

  auto *bitsTitle = new QLabel("IEEE 754 (128 bitów)", bitsFrame);
  bitsTitle->setStyleSheet("color: #E8ECF3; font-size: 15px; font-weight: 600;");
  bitsLayout->addWidget(bitsTitle);

  bits_ = new QPlainTextEdit(bitsFrame);
  bits_->setReadOnly(true);
  bits_->setMinimumHeight(260);
  bits_->setLineWrapMode(QPlainTextEdit::NoWrap);
  bits_->setFont(monoFont);
  bits_->setStyleSheet(
      "QPlainTextEdit { background: #0F1626; color: #C9D6EA;"
      " border: none; border-radius: 8px; padding: 10px; }");
  bitsLayout->addWidget(bits_);
  layout->addWidget(bitsFrame);

  setCentralWidget(central);

  connect(convertBtn, &QPushButton::clicked, this,
          &ConverterWindow::HandleConvert);
  connect(clearBtn, &QPushButton::clicked, this,
          &ConverterWindow::HandleClear);
  connect(input_, &QLineEdit::returnPressed, this,
          &ConverterWindow::HandleConvert);
  connect(input_, &QLineEdit::inputRejected, this,
          &ConverterWindow::HandleInputRejected);
  connect(input_, &QLineEdit::textEdited, this,
          &ConverterWindow::HandleInputTextEdited);

  RenderPlaceholder();
}

void ConverterWindow::HandleConvert() {
  const string raw = input_->text().toStdString();
  ConversionResult result = ConvertToIEEE(raw);
  if (!result.success) {
    SetStatus(QString::fromStdString(result.errorMessage), "#C84C5C");
    RenderPlaceholder();
    return;
  }

  SetStatus("Konwersja zakończona powodzeniem.", "#5BA37C");
  RenderResult(result);
}

void ConverterWindow::HandleClear() {
  input_->clear();
  SetStatus("Wpisz liczbę dziesiętną i kliknij Konwertuj.", "#28344B");
  RenderPlaceholder();
}

void ConverterWindow::RenderResult(const ConversionResult &result) {
  QString summaryText = QString("Bity części całkowitej: %1\nWykładnik (podstawa 2): %2")
                            .arg(TruncateWithDots(QString::fromStdString(result.integerBits),
                                 SUMMARY_TEXT_INTEGER_BITS_MAX_CHARS))
                            .arg(result.exponentValue);
  summary_->setText(summaryText);
  strip_->setText(BuildStripLabel(result));
  mantissa_->setPlainText(BuildGroupedBitLines(result.mantissaBits, 32));
  bits_->setPlainText(BuildGroupedBitLines(result.ieeeBits));
}

void ConverterWindow::RenderPlaceholder() {
  summary_->setText("Wynik pojawi się po konwersji.");
  strip_->setText("Znak (1): -\nWykładnik (15): -\nMantysa (112): -");
  mantissa_->setPlainText("Bity mantysy pojawią się tutaj po konwersji.");
  bits_->setPlainText("Użyj Konwertuj, aby zobaczyć zapis 128-bitowy.");
}

void ConverterWindow::SetStatus(const QString &text, const QString &color) {
  status_->setText(text);
  status_->setStyleSheet(
      QString(
          "background: %1; color: #E8ECF3; padding: 10px 12px;"
          "border-radius: 10px; font-size: 14px;")
          .arg(color));
}

void ConverterWindow::HandleInputRejected() {
  if (input_->text().length() == INPUT_MAX_CHARS) {
    SetStatus("Przekroczono limit znaków pola tekstowego", "#7f6626");
  } else {
    SetStatus("Wpisany znak jest niedozwolony w tym miejscu", "#7f6626");
  }
}

void ConverterWindow::HandleInputTextEdited() {
  SetStatus("Wpisz liczbę dziesiętną i kliknij Konwertuj.", "#28344B");
}

} // namespace

int RunGuiMode(int argc, char **argv) {
  QApplication app(argc, argv);
  ConverterWindow window;
  window.show();
  return app.exec();
}

} // namespace gui
