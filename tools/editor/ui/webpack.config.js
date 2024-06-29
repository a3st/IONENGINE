const path = require('path');

const { VueLoaderPlugin } = require('vue-loader');
const FileManagerPlugin = require('filemanager-webpack-plugin');

module.exports = {
    entry: './src/index.js',
    mode: 'development',
    output: {
        filename: 'app.bundle.js',
        path: path.resolve(__dirname, 'dist')
    },
    plugins: [
        new VueLoaderPlugin(),
        new FileManagerPlugin({
            events: {
                onEnd: {
                    copy: [
                        {
                            source: path.resolve(__dirname, 'dist'),
                            destination: path.resolve(__dirname, '..', '..', '..', 'build', 'tools', 'resources')
                        }
                    ]
                }
            }
        })
    ],
    resolve: {
        extensions: ['.tsx', '.ts', '.js', '.vue']
    },
    module: {
        rules: [
            {
                test: /\.vue$/,
                loader: 'vue-loader'
            },
            {
                test: /\.js$/,
                exclude: /node_modules/,
                loader: 'babel-loader'
            },
            {
                test: /\.css$/,
                use: [
                    'vue-style-loader',
                    'css-loader'
                ]
            },
            {
                test: /\.(png|svg|jpg|jpeg|gif|svg)$/i,
                type: 'asset/resource',
                generator: {
                    filename: '[hash][ext][query]'
                }
            },
            {
                test: /\.(woff|woff2|eot|ttf|otf)$/i,
                type: 'asset/resource',
                generator: {
                    filename: '[hash][ext][query]'
                }
            }
        ]
    }
}