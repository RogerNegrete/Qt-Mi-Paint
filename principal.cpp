#include "principal.h"
#include "ui_principal.h"

#define DEFAULT_ANCHO 3

Principal::Principal(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Principal)
{
    ui->setupUi(this);
    // Instanciando la imagen (creando)
    mImagen = new QImage(this->size(),QImage::Format_ARGB32_Premultiplied);
    // Rellenar la imagen de color blanco
    mImagen->fill(Qt::white);
    // Instanciar el Painter a partir de la imagen
    mPainter = new QPainter(mImagen);
    mPainter->setRenderHint(QPainter::Antialiasing);
    // Inicializar otras variables
    mPuedeDibujar = false;
    mColor = Qt::black;
    mAncho = DEFAULT_ANCHO;
    mNumLineas = 0;
    m_Case = LIBRE; //Para que empieze dibujando Libre
}

Principal::~Principal()
{
    delete ui;
    delete mPainter;
    delete mImagen;
}

void Principal::paintEvent(QPaintEvent *event)
{
    // Crear el painter de la ventana principal
    QPainter painter(this);
    // Dibujar la imagen
    painter.drawImage(0, 0, *mImagen);
    // Acepatr el evento
    event->accept();
}

void Principal::mousePressEvent(QMouseEvent *event)
{
    // Levanta la bandera (para que se pueda dibujar)
    mPuedeDibujar = true;
    // Captura la posición (punto x,y) del mouse
    mInicial = event->pos();
    // Check if mPunto1 and mPunto2 are set

    if (!mPunto1.isNull() && !mPunto2.isNull()) {
        mPunto1 = mPunto2 = QPoint();
    }
    if (!mPunto1.isNull()) {
        mPunto2 = event->pos();
        //Verifica SI esta el Checked
        if (ui->actionLineas->isChecked()) {
            m_Case = LINEAS;
        } else if (ui->actionLibre->isChecked()) {
            m_Case = LIBRE;
        } else if (ui->actionRect_nculos->isChecked()) {
            m_Case = CUADRADO;
        } else if (ui->actionCircunferencias->isChecked()) {
            m_Case = CIRCUFERENCIA;
        }

        // Draw the line, square or circle between the two points
        QPen pincel;
        pincel.setColor(mColor);
        pincel.setWidth(mAncho);
        mPainter->setPen(pincel);
        if(m_Case == LINEAS){
            // Capture the mouse position (x, y point)
            mPainter->drawLine(mPunto1, mPunto2);

        } else if(m_Case==LIBRE){
            mPainter->drawLine(mInicial, event->pos());
        } else if (m_Case == CUADRADO) {
            // Captura la posición (punto x,y) del mouse
            QRect rect(mPunto1, mPunto2);
            mPainter->drawRect(rect);
        } else if (m_Case == CIRCUFERENCIA) {
            // Captura la posición (punto x,y) del mouse
            QRect rect(mPunto1, mPunto2);
            mPainter->drawEllipse(rect);
        }

        update();

        // Reinicia los puntos para el próximo dibujo
        mPunto1 = QPoint();
        mPunto2 = QPoint();
    } else {
        mPunto1 = event->pos();
    }

    event->accept();
}


void Principal::mouseMoveEvent(QMouseEvent *event)
{
    // Validar si se puede dibujar
    if ( !mPuedeDibujar ) {
        // Acepta el evento
        event->accept();
        // Salir del método
        return;
    }
    QPen pincel;
    QRect rect;
    mFinal = event->pos();

    pincel.setColor(mColor);
    pincel.setWidth(mAncho);
    // Dibujar una linea
    mPainter->setPen(pincel);

    switch (m_Case) {
    case LINEAS:
        mPainter->drawLine(mPunto1, mPunto2);
        break;
    case LIBRE:
        mPainter->drawLine(mInicial, mFinal);

        break;
    case CUADRADO:
        mPainter->setPen(pincel);
        rect = QRect(mPunto1, mPunto2);  // Solo asigna el valor aquí
        mPainter->drawRect(rect);

        break;
    case CIRCUFERENCIA:
        mPainter->drawEllipse(QRect(mInicial, mFinal));
        break;
    }
    // Mostrar el número de líneas en la barra de estado
    ui->statusbar->showMessage("Número de líneas: " + QString::number(++mNumLineas));
    // Actualizar la interfaz (repinta con paintEvent)
    update();
    // actualizar el punto inicial
    mInicial = mFinal;

}

void Principal::mouseReleaseEvent(QMouseEvent *event)
{
    // Bajar la bandera (no se puede dibujar)
    mPuedeDibujar = false;
    // Aceptar el vento
    event->accept();

}


void Principal::on_actionAncho_triggered()
{
    mAncho = QInputDialog::getInt(this,
                                  "Ancho del pincel",
                                  "Ingrese el ancho del pincel de dibujo",
                                  mAncho,
                                  1, 20);
}

void Principal::on_actionSalir_triggered()
{
    this->close();
}

void Principal::on_actionColor_triggered()
{
    mColor = QColorDialog::getColor(mColor,
                                    this,
                                    "Color del pincel");
}

void Principal::on_actionNuevo_triggered()
{
    mImagen->fill(Qt::white);
    mNumLineas = 0;
    mPunto1 = QPoint();
    mPunto2 = QPoint();
    update();
}

void Principal::on_actionGuardar_triggered()
{
    // Abrir cuadro de diálogo para obtener el nombre del archivo
    QString nombreArchivo = QFileDialog::getSaveFileName(this,
                                                         "Guardar imagen",
                                                         QString(),
                                                         "Imágenes .png (*.png)");
    // Validar que el nombre del archivo no sea vacío
    if ( !nombreArchivo.isEmpty() ){
        // Guardar imagen
        if (mImagen->save(nombreArchivo)){
            // Si todo va bien, muestra un mensaje de información
            QMessageBox::information(this,
                                     "Guardar imagen",
                                     "Archivo almacenado en: " + nombreArchivo);
        } else{
            // Si hay algún error, muestro advertencia
            QMessageBox::warning(this,
                                 "Guardar imagen",
                                 "No se pudo almacenar la imagen.");
        }
    }
}

void Principal::on_actionLineas_triggered()
{
    m_Case = LINEAS;
}


void Principal::on_actionLibre_triggered()
{
    m_Case=LIBRE;
}


void Principal::on_actionRect_nculos_triggered()
{
    m_Case=CUADRADO;
}


void Principal::on_actionCircunferencias_triggered()
{
    m_Case=CIRCUFERENCIA;
}


void Principal::on_actionLineas_checkableChanged(bool checkable)
{
    if(Qt::Checked) {
            m_Case = LINEAS;
        } else {
            // Código que maneja cuando actionLineas se deselecciona
        }
}

