--TEST--
Test vprintf() function : usage variations - unexpected values for args argument
--FILE--
<?php
/* Prototype  : string vprintf(string format, array args)
 * Description: Output a formatted string 
 * Source code: ext/standard/formatted_print.c
*/

/*
 * Test vprintf() when different unexpected values are passed to
 * the '$args' arguments of the function
*/

echo "*** Testing vprintf() : with unexpected values for args argument ***\n";

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
/*1*/	  0,
		  1,
		  12345,
		  -2345,
		
		  // float data
/*5*/	  10.5,
		  -10.5,
		  10.1234567e10,
		  10.7654321E-10,
		  .5,
		
		  // null data
/*10*/	  NULL,
		  null,
		
		  // boolean data
/*12*/	  true,
		  false,
		  TRUE,
		  FALSE,
		
		  // empty data
/*16*/	  "",
		  '',
		
		  // string data
/*18*/	  "string",
		  'string',
		
		  // object data
/*20*/	  new sample(),
		
		  // undefined data
/*21*/	  @$undefined_var,
		
		  // unset data
/*22*/	  @$unset_var,
		
		  // resource data
/*23*/		  $file_handle
);

// loop through each element of the array for args
$counter = 1;
foreach($values as $value) {
  echo "\n-- Iteration $counter --\n";
  $result = vprintf($format,$value);
  echo "\n";
  var_dump($result);
  $counter++;
};

// closing the resource
fclose($file_handle);

?>
===DONE===
--EXPECTF--
*** Testing vprintf() : with unexpected values for args argument ***

-- Iteration 1 --

Warning: vprintf() expects parameter 2 to be array, integer given in %s on line %d

NULL

-- Iteration 2 --

Warning: vprintf() expects parameter 2 to be array, integer given in %s on line %d

NULL

-- Iteration 3 --

Warning: vprintf() expects parameter 2 to be array, integer given in %s on line %d

NULL

-- Iteration 4 --

Warning: vprintf() expects parameter 2 to be array, integer given in %s on line %d

NULL

-- Iteration 5 --

Warning: vprintf() expects parameter 2 to be array, float given in %s on line %d

NULL

-- Iteration 6 --

Warning: vprintf() expects parameter 2 to be array, float given in %s on line %d

NULL

-- Iteration 7 --

Warning: vprintf() expects parameter 2 to be array, float given in %s on line %d

NULL

-- Iteration 8 --

Warning: vprintf() expects parameter 2 to be array, float given in %s on line %d

NULL

-- Iteration 9 --

Warning: vprintf() expects parameter 2 to be array, float given in %s on line %d

NULL

-- Iteration 10 --

Warning: vprintf() expects parameter 2 to be array, null given in %s on line %d

NULL

-- Iteration 11 --

Warning: vprintf() expects parameter 2 to be array, null given in %s on line %d

NULL

-- Iteration 12 --

Warning: vprintf() expects parameter 2 to be array, boolean given in %s on line %d

NULL

-- Iteration 13 --

Warning: vprintf() expects parameter 2 to be array, boolean given in %s on line %d

NULL

-- Iteration 14 --

Warning: vprintf() expects parameter 2 to be array, boolean given in %s on line %d

NULL

-- Iteration 15 --

Warning: vprintf() expects parameter 2 to be array, boolean given in %s on line %d

NULL

-- Iteration 16 --

Warning: vprintf() expects parameter 2 to be array, string given in %s on line %d

NULL

-- Iteration 17 --

Warning: vprintf() expects parameter 2 to be array, string given in %s on line %d

NULL

-- Iteration 18 --

Warning: vprintf() expects parameter 2 to be array, string given in %s on line %d

NULL

-- Iteration 19 --

Warning: vprintf() expects parameter 2 to be array, string given in %s on line %d

NULL

-- Iteration 20 --

Warning: vprintf() expects parameter 2 to be array, object given in %s on line %d

NULL

-- Iteration 21 --

Warning: vprintf() expects parameter 2 to be array, null given in %s on line %d

NULL

-- Iteration 22 --

Warning: vprintf() expects parameter 2 to be array, null given in %s on line %d

NULL

-- Iteration 23 --

Warning: vprintf() expects parameter 2 to be array, resource given in %s on line %d

NULL
===DONE===
