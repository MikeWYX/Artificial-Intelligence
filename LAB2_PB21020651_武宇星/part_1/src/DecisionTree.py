from sklearn.datasets import load_iris
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import LabelEncoder
import numpy as np 
import pandas as pd 

# metrics
def accuracy(y_true, y_pred):
    return sum(y_true == y_pred) / len(y_true)

class Node():
    def __init__(self, feature=None, threshold=None, left=None, right=None, gain=None, value=None):
        self.feature = feature
        self.threshold = threshold
        self.left = left
        self.right = right
        self.value = value

# model
class DecisionTreeClassifier:
    def __init__(self) -> None:
        self.tree = None

    def split_data(self, dataset, feature, threshold):
        left_dataset = []
        right_dataset = []
        for row in dataset:
            if row[feature] <= threshold:
                left_dataset.append(row)
            else:
                right_dataset.append(row)
        left_dataset = np.array(left_dataset)
        right_dataset = np.array(right_dataset)
        return left_dataset, right_dataset

    def entropy(self, y):
        entropy = 0
        labels = np.unique(y)
        for label in labels:
            label_examples = y[y == label]
            pl = len(label_examples) / len(y)
            entropy += -pl * np.log2(pl)
        return entropy

    def information_gain(self, parent, left, right):
        information_gain = 0
        parent_entropy = self.entropy(parent)
        weight_left = len(left) / len(parent)
        weight_right= len(right) / len(parent)
        entropy_left, entropy_right = self.entropy(left), self.entropy(right)
        weighted_entropy = weight_left * entropy_left + weight_right * entropy_right
        information_gain = parent_entropy - weighted_entropy
        return information_gain

    
    def best_split(self, dataset, num_samples, num_features):
        best_split = {'gain':- 1, 'feature': None, 'threshold': None, 'left_dataset': None, 'right_dataset': None}
        if num_samples > 1:
            for feature_index in range(num_features):
                feature_values = dataset[:, feature_index]
                thresholds = np.unique(feature_values)
                for threshold in thresholds:
                    left_dataset, right_dataset = self.split_data(dataset, feature_index, threshold)
                    if len(left_dataset) and len(right_dataset):
                        y, left_y, right_y = dataset[:, -1], left_dataset[:, -1], right_dataset[:, -1]
                        information_gain = self.information_gain(y, left_y, right_y)
                        if information_gain > best_split["gain"]:
                            best_split["feature"] = feature_index
                            best_split["threshold"] = threshold
                            best_split["left_dataset"] = left_dataset
                            best_split["right_dataset"] = right_dataset
                            best_split["gain"] = information_gain
        return best_split
    
    def calculate_leaf_value(self, y):
        y = list(y)
        most_occuring_value = max(y, key=y.count)
        return most_occuring_value

    def build_tree(self, dataset):
        X, y = dataset[:, :-1], dataset[:, -1]
        n_samples, n_features = X.shape
        best_split = self.best_split(dataset, n_samples, n_features)

        if best_split["gain"] and best_split["threshold"] is not None:
            left_node = self.build_tree(best_split["left_dataset"])
            right_node = self.build_tree(best_split["right_dataset"])
            return Node(best_split["feature"], best_split["threshold"], left_node, right_node)

        leaf_value = self.calculate_leaf_value(y)
        return Node(value=leaf_value)

    def fit(self, X, y):
        y = y.reshape(-1, 1)
        dataset = np.concatenate((X, y), axis=1) 
        self.root = self.build_tree(dataset)
    
    def make_prediction(self, x, node):
        if node.value != None: 
            return node.value
        else:
            feature = x[node.feature]
            if feature <= node.threshold:
                return self.make_prediction(x, node.left)
            else:
                return self.make_prediction(x, node.right)

    def predict(self, X):
        predictions = []
        for x in np.array(X):
            prediction = self.make_prediction(x, self.root)
            predictions.append(prediction)
        np.array(predictions)
        return predictions
    
def load_data(datapath:str='./data/ObesityDataSet_raw_and_data_sinthetic.csv'):
    df = pd.read_csv(datapath)
    continue_features = ['Age', 'Height', 'Weight', 'FCVC', 'NCP', 'CH2O', 'FAF', 'TUE']
    discrete_features = ['Gender', 'CALC', 'FAVC', 'SCC', 'SMOKE', 'family_history_with_overweight', 'CAEC', 'MTRANS']
    discrete_features_size = {'Gender':2, 'CALC':4, 'FAVC':2, 'SCC':2, 'SMOKE':2, 'family_history_with_overweight':2, 'CAEC':4, 'MTRANS':5}
    
    X, y = df.iloc[:, :-1], df.iloc[:, -1]
    # encode discrete str to number, eg. male&female to 0&1
    labelencoder = LabelEncoder()
    for col in discrete_features:
        X[col] = labelencoder.fit(X[col]).transform(X[col])
    y = labelencoder.fit(y).fit_transform(y)
    
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

    return X_train, X_test, y_train, y_test

if __name__=="__main__":
    X_train, X_test, y_train, y_test = load_data('./data/ObesityDataSet_raw_and_data_sinthetic.csv')

    clf = DecisionTreeClassifier()
    clf.fit(X_train, y_train)
    
    y_pred = clf.predict(X_test)
    print(accuracy(y_test, y_pred))
