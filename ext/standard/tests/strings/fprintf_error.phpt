--TEST--
Test fprintf() function (errors)
--FILE--
<?php

/* Testing Error Conditions */
echo "*** Testing Error Conditions ***\n";

/* zero argument */
var_dump( fprintf() );

/* scalar argument */
var_dump( fprintf(3) );

/* NULL argument */
var_dump( fprintf(NULL) );

echo "Done\n";
?>
--EXPECTF--
*** Testing Error Conditions ***

Warning: fprintf() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: fprintf() expects at least 2 parameters, 1 given in %s on line %d
NULL

Warning: fprintf() expects at least 2 parameters, 1 given in %s on line %d
NULL
Done
