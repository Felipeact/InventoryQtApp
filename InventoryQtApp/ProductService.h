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
    

public:
    ProductService(ApiClient& apiClient);

    bool createProduct(const std::string& name, const std::string& barcode, int quantity);
    json getProducts(bool forceRefresh = false);

	bool updateProduct(const std::string& productId, const std::string& name, const std::string& barcode, int quantity);

	bool deleteProduct(const std::string& productId);

    json searchProducts(const std::string& searchText);

    bool scanIn(const std::string& barcode, int quantity);
	bool scanOut(const std::string& barcode, int quantity);

    json getLowStockProducts();
};

