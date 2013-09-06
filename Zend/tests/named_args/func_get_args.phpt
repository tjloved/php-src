--TEST--
func_get_args() skips arguments that weren't passed
--FILE--
<?php

function test($a, $b = "b", $c = "c", ...$rest) {
    var_dump(func_get_args());
    var_dump(func_get_arg(0), func_get_arg(1), func_get_arg(2), func_get_arg(3));
    var_dump(func_num_args());
}

test(c => "C", a => "A");
test(c => "C", a => "A", d => "D");

?>
--EXPECTF--
array(2) {
  [0]=>
  string(1) "A"
  [2]=>
  string(1) "C"
}

Warning: func_get_arg():  Argument 1 not passed to function in %s on line %d

Warning: func_get_arg():  Argument 3 not passed to function in %s on line %d
string(1) "A"
bool(false)
string(1) "C"
bool(false)
int(3)
array(2) {
  [0]=>
  string(1) "A"
  [2]=>
  string(1) "C"
}

Warning: func_get_arg():  Argument 1 not passed to function in %s on line %d

Warning: func_get_arg():  Argument 3 not passed to function in %s on line %d
string(1) "A"
bool(false)
string(1) "C"
bool(false)
int(3)
