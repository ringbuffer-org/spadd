/**
 * \class Window
 *
 *
 * \brief The Window class generates different window types.
 *
 *
 * \author Hilko Tondock
 *
 * \version  0.1
 *
 * \date   2023/03/31
 *
 * Contact: h.tondock@campus.tu-berlin.de
 *
 */

#pragma once

#include <cmath>
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif
#include <vector>
#include <algorithm>
#include <array>
#include <iostream>

enum class WindowType
{
    Triangular = 0,
    Hann,
    BlackmanHarris3term,
    BlackmanHarris4term,
    Nuttall4term,
    BlackmanNuttall4term,
    BlackmanHarris5term,
    BlackmanHarris7term,
    Kaiser
};

template <typename T>
class Window
{
public:
    Window(WindowType windowType, 
           int size,
           bool symmetric)
    {
        T beta = 8.0;

        createWindow(windowType, size, beta, symmetric);
    }
    
    Window(WindowType windowType,
           int size,
           T beta,
           bool symmetric)
    {
        createWindow(windowType, size, beta, symmetric);
    }
    
    void createWindow(WindowType windowType, 
                      int size,
                      T beta,
                      bool symmetric)
    {
        m_window.assign(size, 0);
        createWindow(m_window, windowType, size, beta, symmetric);
    }

    static void createWindow(std::vector<T>& destination, 
                             WindowType windowType, 
                             int size,
                             T beta,
                             bool symmetric) 
    {
        int newSize = symmetric ? size - 1 : size;

        switch (windowType)
        {
            // TODO: Implement periodic and correct form for odd window length! 
            case WindowType::Triangular:
            {
                T halfSize = size * 0.5;
                T offset = 1 / static_cast<T>(size);

                for (int i = 0; i < halfSize; ++i)
                {
                    destination[i] = static_cast<T>(i) / halfSize + offset; 
                    destination[size-i-1] = static_cast<T>(i) / halfSize + offset;
                } 
            }
            break;

            case WindowType::Hann:
            {
                for (int i = 0; i < newSize; ++i)
                    destination[i] = 0.5 - 0.5 * cos(2.0 * M_PI * static_cast<T>(i) / static_cast<T>(newSize));
            }  
            break;

            case WindowType::BlackmanHarris3term:
            {
                std::vector<T> coeff = {0.4243801, 0.4973406, 0.0782793};

                m_calcBlackman(newSize, coeff, destination);
            }
            break;

            case WindowType::BlackmanHarris4term:
            {
                std::vector<T> coeff = {0.35875, 0.48829, 0.14128, 0.01168};
            
                m_calcBlackman(newSize, coeff, destination);
            }
            break;

            case WindowType::Nuttall4term:
            {
                std::vector<T> coeff = {0.355768, 0.487396, 0.144323, 0.012604};
            
                m_calcBlackman(newSize, coeff, destination);
            }
            break;

            case WindowType::BlackmanNuttall4term:
            {
                std::vector<T> coeff = {0.3635819, 0.4891775, 0.1365995, 0.0106411};
            
                m_calcBlackman(newSize, coeff, destination);
            }
            break;

            case WindowType::BlackmanHarris5term:
            {
                std::vector<T> coeff = {0.3232153788877343, 0.4714921439576260, 0.1755341299601972, 0.0284969901061499, 0.0012613570882927};
            
                m_calcBlackman(newSize, coeff, destination);
            }
            break;

            case WindowType::BlackmanHarris7term:
            {
                std::vector<T> coeff = {0.27105140069342, 0.43329793923448, 0.21812299954311, 0.06592544638803, 0.01081174209837, 0.00077658482522, 0.00001388721735};
            
                m_calcBlackman(newSize, coeff, destination);
            }
            break;

            case WindowType::Kaiser:
            {
                m_calcKaiser(newSize, beta, destination);
            }
            break;

            default:
            {
                for (size_t i = 0; i < size; ++i)
                    destination[i] = 1.0;
            }
            break;
        }

        if (symmetric)
            destination[size-1] = destination[0];
    }

    void normalize()
    {
        T sum = 0;

        for (const auto& sample: m_window)
            sum += sample;

        for (auto& sample: m_window)
            sample /= sum;
    }

    void zeroPhase()
    {
        int halfSize = m_window.size() / 2;

        auto buffer1 = std::vector<T>(m_window.begin() + halfSize, m_window.end());
        auto buffer2 = std::vector<T>(m_window.begin(), m_window.begin() + halfSize);

        m_window.clear();

        std::move(buffer1.begin(), buffer1.end(), std::back_inserter(m_window));
        std::move(buffer2.begin(), buffer2.end(), std::back_inserter(m_window));   
    }

    void zeroPad(int size, bool zeroPhasePad)
    {
        const int currentSize = m_window.size(); 
        const int paddedSize = size - currentSize; 

        if (zeroPhasePad)
        {
            const int halfSize = (currentSize % 2) ? (static_cast<T>(currentSize) * 0.5 + 1)
                                                   : (static_cast<T>(currentSize) * 0.5);
            auto itPos = m_window.begin() + halfSize;
            m_window.insert(itPos, paddedSize, 0.0);
        }
        else
        {
            m_window.resize(currentSize + paddedSize, 0.0);
        }
    }

    std::vector<T> getWindow()
    {
        return m_window;
    }

private:
    std::vector<T> m_window;
    
    static void m_calcBlackman(int size, std::vector<T>& coeff, std::vector<T>& destination)
    {
        T df = 2.0 * M_PI / static_cast<T>(size);
            
        for (int i = 0; i < size; ++i)
        {
            for (int m = 0; m < coeff.size(); ++m)
                destination[i] = destination[i] + pow(-1.0, m) * coeff[m] * cos(df * m * i); 
        }
    }
    
    static void m_calcKaiser(int size, T beta, std::vector<T>& destination)
    {
        T fct = 1.0;
        T divisor = 0.0;

        for(int k = 0; k < 16; fct *= ++k)
        {
            divisor += std::pow((beta / 2), 2 * k) / std::pow(fct, 2);
        }                

        for (int i = 0; i < size; ++i)
        {
            T fct = 1;
            T sum = 0;
            T x = beta * sqrt(1 - (2 * static_cast<T>(i) / static_cast<T>(size) - 1) * (2 * static_cast<T>(i) / static_cast<T>(size) - 1));
            
            for(int k = 0; k < 16; fct *= ++k)
            {
                sum += std::pow((x / 2), 2 * k) / std::pow(fct, 2);
            }                

            destination[i] = sum / divisor;
        }
    }
};

template class Window<float>;
template class Window<double>;