## Linear Regression:

1. The data is checked for null values, then normalised using z-score normalisation, which is performed by the `normalise(range)` function that takes a column as input.  
2. The `train_test_split()` function from scikitlearn is used to split the data into a training and testing dataset.  
3. w and b (the coefficient vector and the bias value) are initialised to be zero.  
4. The gradient descent function works as follows:
    * It takes as input the x and y values. These are the values of the features and true outputs from the training dataset.  
    * It also takes as input the initial w vector and initial b value, the number of iterations(epochs) and the learning rate(alpha).
    * Inside the function, a for loop runs for the number of iterations specified.Vectorisation is used to simplify calculations.  
    * At every iteration, the predicted value of y is calculated. Then, the gradients for w and b are found, and w and b are updated.  
    * We then calculate cost for those values of w and b. The cost function here is the mean square error. Next, the function updates `cost_val`, which is an array holding the values of cost for every iteration.  
    * The function returns the optimum values of w and b along with the list of cost values.  
      

5. The accuracy score is calculated using the `r2_score` function from scikitlearn and then printed as a percentage(by multiplying by 100)    

6. The learning curve(which shows variation in cost with iterations of gradient descent) is plotted using `matplotlib`.
