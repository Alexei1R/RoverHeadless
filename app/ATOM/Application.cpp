//
// Created by toor on 11/14/23.
//
#include "Application.h"



namespace Atom {
    Application* Application::s_Instance = nullptr;

    std::pair<std::string, std::string> extractStrings(const std::string& message) {
        std::string firstString, secondString;

        size_t firstPos = message.find('%');
        size_t secondPos = message.find('%', firstPos + 1);
        size_t thirdPos = message.find('$', secondPos + 1);

        if (firstPos != std::string::npos && secondPos != std::string::npos && thirdPos != std::string::npos) {
            firstString = message.substr(firstPos + 1, secondPos - firstPos - 3);
            secondString = message.substr(secondPos + 1, thirdPos - secondPos - 1);
        }

        return {firstString, secondString};
    }

    Application::Application() {
        signal(SIGINT, Application::SignalHandler);
        signal(SIGTERM, Application::SignalHandler);

        s_Instance = (Application *) this;

        m_Interval = std::chrono::milliseconds(33);
        m_ServerLayer = new ServerLayer(27020);
        PushLayer(m_ServerLayer);

        m_Frame = new Frame();
        PushLayer(m_Frame);


        m_ServerLayer->SetServerConnectedCallback([&](std::string ip) {
            ATLOG_INFO("Connected to client: {0}", ip);
            for (CameraUsers& cameraUser: m_CameraUsers) {
                if (cameraUser.ip == ip) {
                    return;
                }
            }
            CameraUsers cameraUser;
            cameraUser.ip = ip;
            cameraUser.IsCreatedVideoWriter = false;
            m_CameraUsers.push_back(cameraUser);
        });
        m_ServerLayer->SetServerDisconnectedCallback([&](std::string ip) {
            ATLOG_INFO("Disconnected from client: {0}", ip);
            for (int i = 0; i < m_CameraUsers.size(); i++) {
                if (m_CameraUsers[i].ip == ip) {
                    m_Frame->RemoveVideoWriterWithIP(ip);
                    m_CameraUsers.erase(m_CameraUsers.begin() + i);
                    return;
                }
            }
        });


        //id 50 , string camera pipeline
        m_ServerLayer->RegisterMessageWithID(50, [&](Message message) {
            std::string pipeline = (char*)message.payload;
            ATLOG_INFO("Message Received: ID = 50 {0}", pipeline);
            if(!m_IsCameraOpen) {
                if (pipeline[0] == '#') {
                    std::pair<std::string, std::string> sources = extractStrings(pipeline);
                    ATLOG_INFO("First source: {0}", sources.first);
                    ATLOG_INFO("Second source: {0}", sources.second);
                     m_Frame->Open2Cameras(sources.first, sources.second);
                } else {
                    ATLOG_INFO("Opening camera with pipeline: {0}", pipeline);
                    m_Frame->OpenCamera(pipeline);
                }





                m_IsCameraOpen = true;
            }


            // for last user that isCameraOpen if false open the camera with ip and pipeline
            for (CameraUsers& cameraUser: m_CameraUsers) {
                if (!cameraUser.IsCreatedVideoWriter) {
                    m_Frame->PushNewVideoWriterWithIP(cameraUser.ip);
                    cameraUser.IsCreatedVideoWriter = true;
                    // send aknowledgement
                     Message response;
                     response.id = 50;
                     response.payloadSize = 2;
                     response.payload = (void*)"OK";
                     m_ServerLayer->SendMessage(response);
                }
            }

        });



    }


    Application::~Application() {

        if (m_ServerLayer) {
            delete m_ServerLayer;
        }
        if (m_Frame) {
            delete m_Frame;
        }


    }


    void Application::Run() {
        while (m_IsRuning) {
            for (Layer* layer: m_LayerStack) {
                layer->OnUpdate();
            }
            std::chrono::time_point<std::chrono::high_resolution_clock> m_CurrentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float, std::milli> m_TimeSpan = m_CurrentTime - m_LastTime;



            if (m_TimeSpan.count() >= m_Interval.count()) {
                for (Layer* layer: m_LayerStack) {
                    layer->OnFixedUpdate();
                }
                m_LastTime = m_CurrentTime;
            }


        }
    }


    void Application::WindowClose() {
        m_IsRuning = false;
    }


    void Application::PushLayer(Layer* layer) {
        m_LayerStack.PushLayer(layer);
    }

    void Application::PushOverlay(Layer* layer) {
        m_LayerStack.PushOverlay(layer);
    }

    void Application::SignalHandler(int signal) {
        if (signal == SIGINT || signal == SIGTERM) {
            std::cout << "Signal (" << signal << ") received. Shutting down." << std::endl;
            Application::GetApp().WindowClose(); // Set the running flag to false
        }
    }
}
