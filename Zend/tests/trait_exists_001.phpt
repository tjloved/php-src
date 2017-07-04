--TEST--
Testing trait_exists()
--FILE--
<?php

trait foo
{
}
function fn1764816528()
{
    var_dump(trait_exists('foo'));
    var_dump(trait_exists(1));
    var_dump(trait_exists(NULL));
    var_dump(trait_exists(new stdClass()));
}
fn1764816528();
--EXPECTF--
bool(true)
bool(false)
bool(false)

Warning: trait_exists() expects parameter 1 to be string, object given in %s on line %d
NULL
