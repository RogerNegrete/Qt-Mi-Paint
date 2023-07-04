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

    if (!mPunto1.isNull()) {
        mPunto2 = event->pos();

        // Dibuja el rectángulo o la circunferencia entre los dos puntos
        QPen pincel;
        pincel.setColor(mColor);
        pincel.setWidth(mAncho);
        mPainter->setPen(pincel);

        if (m_Case == CUADRADO) {
           // QRect rect(mPunto1, mPunto2);
            //mPainter->drawRect(rect);
        } else if (m_Case == CIRCUFERENCIA) {
            QRect rect(mPunto1, mPunto2);
            mPainter->drawEllipse(rect);


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

    pincel.setColor(mColor);
    pincel.setWidth(mAncho);
    // Dibujar una linea
    mPainter->setPen(pincel);

    switch (m_Case) {
    case LINEAS:
 mFinal = event->pos();
        mPainter->drawLine(mPunto1, mPunto2);
            mInicial = mFinal;
        break;
    case LIBRE:
        // Capturar el punto a donde se mueve el mouse
        mFinal = event->pos();
        // Crear un pincel y establecer atributos


        mPainter->drawLine(mInicial, mFinal);
        update();
        // actualizar el punto inicial
        mInicial = mFinal;

        break;
    case CUADRADO:
        mPainter->setPen(pincel);
        QRect rect(mPunto1, mPunto2);
        mPainter->drawRect(rect);
        mFinal = event->pos();
        break;
    case CIRCUFERENCIA:
        mFinal = event->pos();
        mPainter->drawEllipse(QRect(mInicial, mFinal));
           mInicial = mFinal;
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

