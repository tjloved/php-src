--TEST--
Bug #73181: parse_str() without a second argument leads to crash
--FILE--
<?php

function x()
{
    parse_str("1&x");
    var_dump(get_defined_vars());
}
function fn2056321995()
{
    x();
}
fn2056321995();
--EXPECTF--
Deprecated: parse_str(): Calling parse_str() without the result argument is deprecated in %s on line %d
array(2) {
  [1]=>
  string(0) ""
  ["x"]=>
  string(0) ""
}
