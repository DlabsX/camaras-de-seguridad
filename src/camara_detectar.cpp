#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>

int main() {
    std::vector<int> indicesDisponibles;
    int maxCamaras = 10;

    std::cout << "🔍 Detectando cámaras disponibles...\n";

    for (int i = 0; i < maxCamaras; ++i) {
        cv::VideoCapture cam(i);
        if (cam.isOpened()) {
            indicesDisponibles.push_back(i);
            cam.release();
            std::cout << "✅ Cámara disponible en el índice " << i << "\n";
        }
    }

    if (indicesDisponibles.empty()) {
        std::cerr << "❌ No se encontró ninguna cámara\n";
        return -1;
    }

    std::vector<cv::VideoCapture> cams;
    std::vector<std::string> nombresVentanas;

    std::cout << "\n📋 ¿Qué cámaras deseas activar?\n";
    for (int i : indicesDisponibles) {
        std::cout << "¿Activar cámara " << i << "? (1 = sí, 0 = no): ";
        int respuesta;
        std::cin >> respuesta;
        if (respuesta == 1) {
            cv::VideoCapture cam(i);
            if (cam.isOpened()) {
                cams.push_back(std::move(cam));
                std::string nombre = "Camara " + std::to_string(i);
                nombresVentanas.push_back(nombre);
                cv::namedWindow(nombre);
                std::cout << "✅ Cámara " << i << " activada\n";
            }
        }
    }

    std::cout << "\n🎥 Presiona 'q' para cerrar todas las cámaras\n";

    while (true) {
        for (size_t i = 0; i < cams.size(); ++i) {
            cv::Mat frame;
            cams[i].read(frame);
            if (!frame.empty()) {
                cv::imshow(nombresVentanas[i], frame);
            }

            if (cv::getWindowProperty(nombresVentanas[i], cv::WND_PROP_VISIBLE) < 1) {
                cams[i].release();
                cv::destroyWindow(nombresVentanas[i]);
            }
        }

        if (cv::waitKey(1) == 'q') break;
    }

    for (size_t i = 0; i < cams.size(); ++i) {
        cams[i].release();
        cv::destroyWindow(nombresVentanas[i]);
    }

    return 0;
}