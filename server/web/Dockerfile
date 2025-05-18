FROM node:18-alpine
WORKDIR /app
COPY package.json .
COPY views .
RUN npm install --omit=dev
COPY . .
EXPOSE 4000
CMD ["node","server.js"]
