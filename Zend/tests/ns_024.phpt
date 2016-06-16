--TEST--
024: __NAMESPACE__ constant out of namespace
--FILE--
<?php

function fn842326611()
{
    var_dump(__NAMESPACE__);
}
fn842326611();
--EXPECT--
string(0) ""
