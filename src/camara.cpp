#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::VideoCapture cam(0); // 0 = cámara predeterminada
    if (!cam.isOpened()) {
        std::cerr << "❌ No se pudo abrir la cámara\n";
        return -1;
    }

    cv::Mat frame;
    std::cout << "🎥 Presiona 'q' para salir\n";

    while (true) {
        cam.read(frame); // Capturar imagen de la cámara

        if (frame.empty()) {
            std::cerr << "⚠️ No se pudo capturar el frame\n";
            break;
        }

        cv::imshow("Camara", frame);

        // Detectar si se cerró la ventana manualmente
        if (cv::getWindowProperty("Camara", cv::WND_PROP_VISIBLE) < 1) break;

        // Salir si se presiona 'q'
        if (cv::waitKey(1) == 'q') break;
    }

    cam.release();
    cv::destroyAllWindows();
    return 0;
}