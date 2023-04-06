#pragma once

#include <cmath>
#include <array>
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

enum class Normalisation
{
    SN3D = 1,
    N3D
};

template <typename T>
struct Partial
{
    // Member
    T amplitude = { 1.0 };
    T frequency = { 440.0 };
    T phase = { 0.0 };
    T distance = { 1.0 };
    T azimuth = { 0.0 };
    T elevation = { 0.0 };

    std::array<T, 16> bFormat = {}; 
   
    // Methods
    void setBFormat(Normalisation normalisation)
    {
        switch (normalisation) 
        {
            case Normalisation::SN3D:
            {
                T dist = 1.0 / distance;
                T sinAzimuth = sin(azimuth);
                T cosAzimuth = cos(azimuth);
                T sin2Azimuth = sin(2.0 * azimuth);
                T cos2Azimuth = cos(2.0 * azimuth);
                T sin3Azimuth = sin(3.0 * azimuth);
                T cos3Azimuth = cos(3.0 * azimuth);
                T sinElevation = sin(elevation);
                T cosElevation = cos(elevation);
                T sin2Elevation = sin(2.0 * elevation);

                bFormat[0] = dist * 1.0;
                bFormat[1] = dist * sinAzimuth * cosElevation; 
                bFormat[2] = dist * sinElevation;
                bFormat[3] = dist * cosAzimuth * cosElevation; 
                bFormat[4] = dist * 0.8660254038 * sin2Azimuth * cosElevation * cosElevation; 
                bFormat[5] = dist * 0.8660254038 * sinAzimuth * sin2Elevation; 
                bFormat[6] = dist * 0.5 * (3.0 * sinElevation * sinElevation - 1.0); 
                bFormat[7] = dist * 0.8660254038 * cosAzimuth * sin2Elevation;
                bFormat[8] = dist * 0.8660254038 * cos2Azimuth * cosElevation * cosElevation; 
                bFormat[9] = dist * 0.790569415 * sin3Azimuth * cosElevation * cosElevation * cosElevation; 
                bFormat[10] = dist * 1.936491673 * sin2Azimuth * sinElevation * cosElevation * cosElevation; 
                bFormat[11] = dist * 0.6123724357 * sinAzimuth * (5.0 * sinElevation * sinElevation - 1.0) * cosElevation; 
                bFormat[12] = dist * 0.5 * sinElevation * (5.0 * sinElevation * sinElevation - 3.0); 
                bFormat[13] = dist * 0.6123724357 * cosAzimuth * (5.0 * sinElevation * sinElevation - 1.0) * cosElevation; 
                bFormat[14] = dist * 1.936491673 * cos2Azimuth * sinElevation * cosElevation * cosElevation; 
                bFormat[15] = dist * 0.790569415 * cos3Azimuth * cosElevation * cosElevation * cosElevation; 
            }
            break;

            case Normalisation::N3D:
            {
                T dist = 1.0 / distance;
                T sinAzimuth = sin(azimuth);
                T cosAzimuth = cos(azimuth);
                T sin2Azimuth = sin(2.0 * azimuth);
                T cos2Azimuth = cos(2.0 * azimuth);
                T sin3Azimuth = sin(3.0 * azimuth);
                T cos3Azimuth = cos(3.0 * azimuth);
                T sinElevation = sin(elevation);
                T cosElevation = cos(elevation);
                T sin2Elevation = sin(2.0 * elevation);

                bFormat[0] = dist * 1.0;
                bFormat[1] = dist * 1.732050808 * sinAzimuth * cosElevation; 
                bFormat[2] = dist * 1.732050808 * sinElevation;
                bFormat[3] = dist * 1.732050808 * cosAzimuth * cosElevation; 
                bFormat[4] = dist * 1.936491673 * sin2Azimuth * cosElevation * cosElevation; 
                bFormat[5] = dist * 1.936491673 * sinAzimuth * sin2Elevation; 
                bFormat[6] = dist * 1.118033989 * (3.0 * sinElevation * sinElevation - 1.0); 
                bFormat[7] = dist * 1.936491673 * cosAzimuth * sin2Elevation;
                bFormat[8] = dist * 1.936491673 * cos2Azimuth * cosElevation * cosElevation; 
                bFormat[9] = dist * 0.7395099729 * sin3Azimuth * cosElevation * cosElevation * cosElevation; 
                bFormat[10] = dist * 5.123475383 * sin2Azimuth * sinElevation * cosElevation * cosElevation; 
                bFormat[11] = dist * 0.5728219619 * sinAzimuth * (5.0 * sinElevation * sinElevation - 1.0) * cosElevation; 
                bFormat[12] = dist * 1.322875656 * sinElevation * (5.0 * sinElevation * sinElevation - 3.0); 
                bFormat[13] = dist * 0.5728219619 * cosAzimuth * (5.0 * sinElevation * sinElevation - 1.0) * cosElevation; 
                bFormat[14] = dist * 5.123475383 * cos2Azimuth * sinElevation * cosElevation * cosElevation; 
                bFormat[15] = dist * 0.7395099729 * cos3Azimuth * cosElevation * cosElevation * cosElevation; 
            }
            break;

            default:
            {
                for (auto& i: bFormat)
                    i = 1.0;
            }
            break;
        }
    }   
};

template struct Partial<float>;
template struct Partial<double>;