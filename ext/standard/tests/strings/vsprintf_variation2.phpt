--TEST--
Test vsprintf() function : usage variations - unexpected values for args argument
--FILE--
<?php
/* Prototype  : string vsprintf(string format, array args)
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

/*
 * Test vsprintf() when different unexpected values are passed to
 * the '$args' arguments of the function
*/

echo "*** Testing vsprintf() : with unexpected values for args argument ***\n";

// initialising the required variables
$format = '%s';

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// declaring a class
class sample
{
  public function __toString() {
  return "object";
  }
}

// Defining resource
$file_handle = fopen(__FILE__, 'r');


//array of values to iterate over
$values = array(

  // int data
  0,
  1,
  12345,
  -2345,

  // float data
  10.5,
  -10.5,
  10.1234567e10,
  10.7654321E-10,
  .5,

  // null data
  NULL,
  null,

  // boolean data
  true,
  false,
  TRUE,
  FALSE,

  // empty data
  "",
  '',

  // string data
  "string",
  'string',

  // object data
  new sample(),

  // undefined data
  @$undefined_var,

  // unset data
  @$unset_var,

  // resource data
  $file_handle
);

// loop through each element of the array for args
$counter = 1;
foreach($values as $value) {
  echo "\n-- Iteration $counter --\n";
  var_dump( vsprintf($format,$value) );
  $counter++;
};

// closing the resource
fclose($file_handle);

echo "Done";
?>
--EXPECTF--
*** Testing vsprintf() : with unexpected values for args argument ***

-- Iteration 1 --

Warning: vsprintf() expects parameter 2 to be array, integer given in %s on line %d
NULL

-- Iteration 2 --

Warning: vsprintf() expects parameter 2 to be array, integer given in %s on line %d
NULL

-- Iteration 3 --

Warning: vsprintf() expects parameter 2 to be array, integer given in %s on line %d
NULL

-- Iteration 4 --

Warning: vsprintf() expects parameter 2 to be array, integer given in %s on line %d
NULL

-- Iteration 5 --

Warning: vsprintf() expects parameter 2 to be array, float given in %s on line %d
NULL

-- Iteration 6 --

Warning: vsprintf() expects parameter 2 to be array, float given in %s on line %d
NULL

-- Iteration 7 --

Warning: vsprintf() expects parameter 2 to be array, float given in %s on line %d
NULL

-- Iteration 8 --

Warning: vsprintf() expects parameter 2 to be array, float given in %s on line %d
NULL

-- Iteration 9 --

Warning: vsprintf() expects parameter 2 to be array, float given in %s on line %d
NULL

-- Iteration 10 --

Warning: vsprintf() expects parameter 2 to be array, null given in %s on line %d
NULL

-- Iteration 11 --

Warning: vsprintf() expects parameter 2 to be array, null given in %s on line %d
NULL

-- Iteration 12 --

Warning: vsprintf() expects parameter 2 to be array, boolean given in %s on line %d
NULL

-- Iteration 13 --

Warning: vsprintf() expects parameter 2 to be array, boolean given in %s on line %d
NULL

-- Iteration 14 --

Warning: vsprintf() expects parameter 2 to be array, boolean given in %s on line %d
NULL

-- Iteration 15 --

Warning: vsprintf() expects parameter 2 to be array, boolean given in %s on line %d
NULL

-- Iteration 16 --

Warning: vsprintf() expects parameter 2 to be array, string given in %s on line %d
NULL

-- Iteration 17 --

Warning: vsprintf() expects parameter 2 to be array, string given in %s on line %d
NULL

-- Iteration 18 --

Warning: vsprintf() expects parameter 2 to be array, string given in %s on line %d
NULL

-- Iteration 19 --

Warning: vsprintf() expects parameter 2 to be array, string given in %s on line %d
NULL

-- Iteration 20 --

Warning: vsprintf() expects parameter 2 to be array, object given in %s on line %d
NULL

-- Iteration 21 --

Warning: vsprintf() expects parameter 2 to be array, null given in %s on line %d
NULL

-- Iteration 22 --

Warning: vsprintf() expects parameter 2 to be array, null given in %s on line %d
NULL

-- Iteration 23 --

Warning: vsprintf() expects parameter 2 to be array, resource given in %s on line %d
NULL
Done
