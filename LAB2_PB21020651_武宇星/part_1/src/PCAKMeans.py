from sklearn.datasets import load_wine
import numpy as np 
import matplotlib.pyplot as plt
from gensim.models import KeyedVectors


def get_kernel_function(kernel:str):
    # TODO: implement different kernel functions 
    if kernel == 'linear':
        return lambda X: X.dot(X.T)
    elif kernel == 'rbf':
        # gamma = 1.0  
        return lambda X: np.exp(-1 / X.shape[1] * np.sum((X[:, np.newaxis] - X[np.newaxis, :]) ** 2, axis=2))
    return None

class PCA:
    def __init__(self, n_components:int=2, kernel:str="rbf") -> None:
        self.n_components = n_components
        self.kernel_f = get_kernel_function(kernel)
        # ...

    def fit(self, X:np.ndarray):
        # X: [n_samples, n_features]
        # TODO: implement PCA algorithm
        K = self.kernel_f(X)
        N = K.shape[0]
        one_n = np.ones((N, N)) / N
        K_centered = K - one_n.dot(K) - K.dot(one_n) + one_n.dot(K).dot(one_n)
              
        eig_vals, eig_vecs = np.linalg.eig(K_centered)         
        idx = eig_vals.argsort()[::-1]
        self.eig_vals = eig_vals[idx][:self.n_components]
        self.eig_vecs = eig_vecs[:, idx][:, :self.n_components]
        return self

    def transform(self, X:np.ndarray):
        # X: [n_samples, n_features]
        X_reduced = np.zeros((X.shape[0], self.n_components))

        # TODO: transform the data to low dimension
        K = self.kernel_f(X)
        N = K.shape[0]
        one_n = np.ones((N, N)) / N
        K_centered = K - one_n.dot(K) - K.dot(one_n) + one_n.dot(K).dot(one_n)
        X_proj = K_centered.dot(self.eig_vecs.dot(np.diag(1 / np.sqrt(self.eig_vals))))        
        return X_proj

class KMeans:
    def __init__(self, n_clusters:int=3, max_iter:int=10) -> None:
        self.n_clusters = n_clusters
        self.max_iter = max_iter
        self.centers = None
        self.labels = None

    # Randomly initialize the centers
    def initialize_centers(self, points):
        # points: (n_samples, n_dims,)
        n, d = points.shape

        self.centers = np.zeros((self.n_clusters, d))
        for k in range(self.n_clusters):
            # use more random points to initialize centers, make kmeans more stable
            random_index = np.random.choice(n, size=10, replace=False)
            self.centers[k] = points[random_index].mean(axis=0)
        
        return self.centers
    
    # Assign each point to the closest center
    def assign_points(self, points):
        # points: (n_samples, n_dims,)
        # return labels: (n_samples, )
        n_samples, n_dims = points.shape
        self.labels = np.zeros(n_samples)
        # TODO: Compute the distance between each point and each center
        # and Assign each point to the closest center
        distances = np.linalg.norm(points[:, np.newaxis] - self.centers, axis=2)
        self.labels = np.argmin(distances, axis=1)
        return self.labels

    # Update the centers based on the new assignment of points
    def update_centers(self, points):
        # points: (n_samples, n_dims,)
        # TODO: Update the centers based on the new assignment of points
        for i in range(self.n_clusters):
            cluster_points = points[self.labels == i]
            if len(cluster_points) > 0:
                self.centers[i] = np.mean(cluster_points, axis=0)

    # k-means clustering
    def fit(self, points):
        # points: (n_samples, n_dims,)
        # TODO: Implement k-means clustering
        self.initialize_centers(points)
        for _ in range(self.max_iter):
            old_centers = self.centers.copy()
            self.assign_points(points)
            self.update_centers(points)
            if np.allclose(old_centers, self.centers):
                break
        return self

    # Predict the closest cluster each sample in X belongs to
    def predict(self, points):
        # points: (n_samples, n_dims,)
        # return labels: (n_samples, )
        return self.assign_points(points)
    
def load_data():
    words = [
        'computer', 'laptop', 'minicomputers', 'PC', 'software', 'Macbook',
        'king', 'queen', 'monarch','prince', 'ruler','princes', 'kingdom', 'royal',
        'man', 'woman', 'boy', 'teenager', 'girl', 'robber','guy','person','gentleman',
        'banana', 'pineapple','mango','papaya','coconut','potato','melon',
        'shanghai','HongKong','chinese','Xiamen','beijing','Guilin',
        'disease', 'infection', 'cancer', 'illness', 
        'twitter', 'facebook', 'chat', 'hashtag', 'link', 'internet',
    ]
    w2v = KeyedVectors.load_word2vec_format('/data/wuyux/GoogleNews-vectors-negative300.bin', binary = True)
    vectors = []
    for w in words:
        vectors.append(w2v[w].reshape(1, 300))
    vectors = np.concatenate(vectors, axis=0)
    return words, vectors

if __name__=='__main__':
    words, data = load_data()
    pca = PCA(n_components=2).fit(data)
    data_pca = pca.transform(data)

    kmeans = KMeans(n_clusters=7).fit(data_pca)
    clusters = kmeans.predict(data_pca)

    # plot the data
    
    plt.figure()
    plt.scatter(data_pca[:, 0], data_pca[:, 1], c=clusters)
    for i in range(len(words)):
        plt.annotate(words[i], data_pca[i, :]) 
    plt.title("PB21020651")
    plt.savefig("PCA_KMeans.png")