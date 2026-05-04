// ProductService.h - Service for product operations
#pragma once
#include "ApiClient.h"
#include <nlohmann/json.hpp>
#include <string>
using json = nlohmann::json;


// Service class for handling product-related operations
class ProductService
{
private:
    ApiClient& api;
    json cachedProducts = json::array();

public:
    ProductService(ApiClient& apiClient);

    bool createProduct(const std::string& name, const std::string& barcode, int quantity);
    json getProducts(bool forceRefresh = false);
};

