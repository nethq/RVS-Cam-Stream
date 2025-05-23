FROM python:3.12-slim
WORKDIR /app
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt
COPY control.json .
COPY templates ./templates
COPY main.py .
EXPOSE 4000
CMD ["uvicorn","main:app","--host","0.0.0.0","--port","4000"]
