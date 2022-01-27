#pragma once

#ifndef Yahoo_h
#define Yahoo_h

#include <string>
#include <curl\curl.h>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include "Matrice.h"
#include <stdio.h>
#include <fstream>
#include <sstream>


enum Interval
{
    WEEKLY,
    MONTHLY,
    DAILY
};

static const std::string EnumAPIValues[]{ "1wk", "1mo", "1d" };

std::string get_api_interval_value(int value)
{
    return EnumAPIValues[value];
}

class YahooFinanceAPI
    {
    public:
        YahooFinanceAPI();
        void set_interval(Interval interval);
        void set_col_name(std::string col_name);
        Matrice get_ticker_data(std::string ticker, std::string start_date, std::string end_date, bool keep_file = false);
        std::vector<string> get_ticker_date(std::string ticker, std::string start_date, std::string end_date);
        

    private:
        std::string _base_url;
        Interval _interval;
        std::string _col_name;

        std::string build_url(std::string ticker, std::string start_date, std::string end_date);
        bool string_replace(std::string& str, const std::string from, const std::string to);
        std::string timestamp_from_string(std::string date);
        void download_file(std::string url, std::string filename);
    };

    YahooFinanceAPI::YahooFinanceAPI()
    {
        this->_base_url =
            "https://query1.finance.yahoo.com/v7/finance/download/{ticker}?period1={start_time}&period2={end_time}&interval={interval}&events=history";
        this->_interval = DAILY;
        this->_col_name = "Open";
    }

    std::string YahooFinanceAPI::timestamp_from_string(std::string date)
    {
        struct std::tm time = { 0,0,0,0,0,0,0,0,0 };
        std::istringstream ss(date);
        ss >> std::get_time(&time, "%Y-%m-%d");
        if (ss.fail())
        {
            std::cerr << "ERROR: Cannot parse date string (" << date << "); required format %Y-%m-%d" << std::endl;
            exit(1);
        }
        time.tm_hour = 0;
        time.tm_min = 0;
        time.tm_sec = 0;
        std::time_t epoch = std::mktime(&time);

        return std::to_string(epoch);
    }

    bool YahooFinanceAPI::string_replace(std::string& str, const std::string from, const std::string to)
    {
        size_t start = str.find(from);
        if (start == std::string::npos)
        {
            return false;
        }
        str.replace(start, from.length(), to);
        return true;
    }

    std::string YahooFinanceAPI::build_url(std::string ticker, std::string start, std::string end)
    {
        std::string url = this->_base_url;
        string_replace(url, "{ticker}", ticker);
        string_replace(url, "{start_time}", timestamp_from_string(start));
        string_replace(url, "{end_time}", timestamp_from_string(end));
        string_replace(url, "{interval}", get_api_interval_value(this->_interval));
        return url;
    }


    void YahooFinanceAPI::set_interval(Interval interval)
    {
        this->_interval = interval;
    }

    void YahooFinanceAPI::set_col_name(std::string name)
    {
        this->_col_name = name;
    }

    void YahooFinanceAPI::download_file(std::string url, std::string filename)
    {
        CURL* curl;
        FILE* fp;
        CURLcode res;
        curl = curl_easy_init();
        if (curl)
        {
            fp = fopen(filename.c_str(), "wb");
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            res = curl_easy_perform(curl);

            /* always cleanup */
            curl_easy_cleanup(curl);
            fclose(fp);
        }
    }


    std::vector <string> YahooFinanceAPI::get_ticker_date(std::string ticker, std::string start, std::string end)
    {
        std::string url = build_url(ticker, start, end);
        std::time_t now = std::time(0); // now
        std::string output_file_name = ticker + "_" + std::to_string(now) + ".csv";
        output_file_name = "tmp_" + output_file_name;

        download_file(url, output_file_name);
        fstream file(output_file_name, ios::in);

        int nbLigne = 0;
        string line, word;
        std::vector<string> content;

        if (file.is_open())
        {
            while (getline(file, line))
            {
                stringstream str(line);
                if (line != "Date,Open,High,Low,Close,Adj Close,Volume")
                    {
                        while (getline(str, word, ','))
                        {
                            content.push_back(word);
                            break;
                        }
                    }
            }
        }
        
        std::remove(output_file_name.c_str());
        return content;
    }
    
    Matrice YahooFinanceAPI::get_ticker_data(std::string ticker, std::string start, std::string end, bool keep_file)
    {
        std::string url = build_url(ticker, start, end);
        std::time_t now = std::time(0); // now
        std::string output_file_name = ticker + "_" + std::to_string(now) + ".csv";
        if (!keep_file)
            output_file_name = "tmp_" + output_file_name;

        download_file(url, output_file_name);
        fstream file(output_file_name, ios::in);

        int nbLigne = 0;
        string line, word;

        if (file.is_open()) {
            while (getline(file, line)) {
                nbLigne = nbLigne + 1;
            }
        }
        file.clear();
        file.seekg(0, std::ios::beg);
        Matrice content = Matrice(nbLigne-1,6);
        int i = 0;
        int j = 0;

        if (file.is_open())
        {
            while (getline(file, line))
            {
                    j = 0;
                    stringstream str(line);
                    if (line == "Date,Open,High,Low,Close,Adj Close,Volume") {
                        i = -1;
                    }
                    else {

                        while (getline(str, word, ','))
                        {
                            content.valeurs[i][j] = std::stod(word);
                            j = j + 1;
                        }
                    }
                i = i + 1;
            }
        }
        if (!keep_file)
            std::remove(output_file_name.c_str());
        return content;
    }


#endif