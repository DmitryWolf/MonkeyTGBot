#!/bin/bash

echo "Building Docker image..."
docker build -t c_tgbot .

if [ $? -eq 0 ]; then
    echo "Build completed successfully."
    
    echo "Running Docker container..."
    docker run -d --restart unless-stopped c_tgbot
    
    if [ $? -eq 0 ]; then
        echo "Container started successfully."
    else
        echo "Error starting the container."
    fi
else
    echo "Error building the image."
fi
