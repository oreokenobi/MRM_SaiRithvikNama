## Logistic Regression:

1. The data is checked for null values.
2. One-hot encoding is done to encode the quality variable and get multiple classes using the `get_dummies()` function from pandas. This function shows boolean values by default, so datatype is specified by using `dtype=int`.
3. The `normalise()` function normalises a range of values using z-score normalisation and is fed a column as input.  
4. A column of ones is added to the feature matrix to function as the bias. This gets rid of the need for a separate bias matrix.
5. The weight matrix is initialised as a matrix of zeroes. This matrix has 11 rows (for every feature) and 6 columns (for every class).    
6. The softmax function returns the probability that an input variable belongs to a particular class. Vectorisation is used here.
7.  The gradient descent function works as follows:
    * It takes as input the x and y values. These are the values of the features and true outputs from the training dataset.  
    * It also takes as input the initial w vector, the number of iterations(also called epochs) and the learning rate(alpha).
    * Inside the function, a for loop runs for the number of iterations specified. Vectorisation is used to simplify calculations. 
    * At every iteration, the predicted value of y is calculated. Then, the gradients for w is found and w is updated. 
    * We then calculate cost for those values of w and b and update `cost_val`, which is an array holding the values of cost for every iteration.   
    * The cost is calculated using a separately defined cost function. The cost function in this case is called cross-entropy loss and it penalises deviations/errors more severely than mean square error, which is used in linear regression.  
    * The function returns the optimum values of w along with the list of cost values.  
    
8. Once the optimum w value is obtained from the function, we calculate `z_predict` which is then passed to the `softmax(z)` function to return the list of probabilities for each class and each input variable. Then, we take the highest probability out of these as the class prediction for that input.  
 
9. The accuracy score is calculated using the `accuracy_score` function by passing the highest probability list (called `prediction_list` here) from scikitlearn and then printed as a percentage(by multiplying by 100).  

10. The learning curve(which shows variation in cost with iterations of gradient descent) is plotted using `matplotlib`.
