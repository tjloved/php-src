--TEST--
024: __NAMESPACE__ constant out of namespace
--FILE--
<?php

function fn1203444012()
{
    var_dump(__NAMESPACE__);
}
fn1203444012();
--EXPECT--
string(0) ""
