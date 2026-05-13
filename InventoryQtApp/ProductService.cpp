// ProductService.cpp - Implementation of product service (TODO: implementation pending)
#include "ProductService.h"
#include <iostream>

ProductService::ProductService(ApiClient& apiClient)
    : api(apiClient)
{
}

bool ProductService::createProduct(const std::string& name, const std::string& barcode, int quantity)
{
    json body = {
        {"name", name},
        {"barcode", barcode},
        {"quantity", quantity}
    };

    auto res = api.post("/products", body.dump());

    if (res.status_code != 200 && res.status_code != 201) {
        std::cout << res.text << std::endl;
        return false;
    }

    cachedProducts = json::array(); // clear cache after change
    return true;
}

json ProductService::getProducts(bool forceRefresh)
{
    auto res = api.get("/products?page=1&limit=100");

    if (res.status_code != 200) {
        return json::array();
    }

    auto response = json::parse(res.text);

    if (response.contains("data")) {
        return response["data"];
    }

    return json::array();
}

bool ProductService::updateProduct(const std::string& productId, const std::string& name, const std::string& barcode, int quantity)
{
    json body = {
         {"name", name},
         {"barcode", barcode},
         {"quantity", quantity}
    };

    auto res = api.put("/products/" + productId, body.dump());

    if (res.status_code != 200) {
        std::cout << res.text << std::endl;
        return false;
    }

    cachedProducts = json::array();
    return true;

}

bool ProductService::deleteProduct(const std::string& productId)
{
    auto res = api.del("/products/" + productId);

    if (res.status_code != 200) {
        std::cout << res.text << std::endl;
        return false;
    }

    cachedProducts = json::array();
    return true;
}
