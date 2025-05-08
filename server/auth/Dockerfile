FROM node:18-alpine
WORKDIR /app
COPY package.json .
RUN npm install --omit=dev
COPY auth.js .
EXPOSE 3000
CMD ["node","auth.js"]
