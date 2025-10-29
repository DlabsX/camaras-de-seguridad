#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>

int main() {
    int cantidad = 5; // Número máximo de cámaras
    std::vector<cv::VideoCapture> cams(cantidad);
    std::vector<std::string> nombresVentanas(cantidad);
    std::vector<bool> activas(cantidad, false);

    std::cout << "📋 Control de cámaras disponibles (0 a " << cantidad - 1 << ")\n";

    // Menú para activar cámaras
    for (int i = 0; i < cantidad; ++i) {
        std::cout << "¿Activar cámara " << i << "? (1 = sí, 0 = no): ";
        int respuesta;
        std::cin >> respuesta;
        if (respuesta == 1) {
            cams[i].open(i);
            if (cams[i].isOpened()) {
                activas[i] = true;
                nombresVentanas[i] = "Camara " + std::to_string(i);
                cv::namedWindow(nombresVentanas[i]);
                std::cout << "✅ Cámara " << i << " activada\n";
            } else {
                std::cerr << "❌ Cámara " << i << " no disponible\n";
            }
        }
    }

    std::cout << "🎥 Presiona 'q' para cerrar todas las cámaras\n";

    // Bucle de visualización
    while (true) {
        for (int i = 0; i < cantidad; ++i) {
            if (activas[i]) {
                cv::Mat frame;
                cams[i].read(frame);
                if (!frame.empty()) {
                    cv::imshow(nombresVentanas[i], frame);
                }

                // Si se cierra manualmente la ventana
                if (cv::getWindowProperty(nombresVentanas[i], cv::WND_PROP_VISIBLE) < 1) {
                    std::cout << "❎ Ventana cerrada: " << nombresVentanas[i] << "\n";
                    cams[i].release();
                    cv::destroyWindow(nombresVentanas[i]);
                    activas[i] = false;
                }
            }
        }

        // Salir si se presiona 'q'
        if (cv::waitKey(1) == 'q') break;
    }

    // Liberar todas las cámaras
    for (int i = 0; i < cantidad; ++i) {
        if (activas[i]) {
            cams[i].release();
            cv::destroyWindow(nombresVentanas[i]);
        }
    }

    return 0;
}