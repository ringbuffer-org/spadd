project(SignalNoiseRatio VERSION 0.0.1)

add_executable(SNR Source/SNR.cpp)

target_sources(SNR PRIVATE
    ../../Plugin/Source/IFFT.cpp
    ../../Plugin/Source/BasicSignals.cpp
    ../../Plugin/Source/SpectralMotif.cpp)

target_link_libraries(SNR PRIVATE
    shared_processing_code
    kfr_dft)
