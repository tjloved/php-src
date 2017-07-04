--TEST--
Testing include
--FILE--
<?php

function fn651310669()
{
    include "015.inc";
}
fn651310669();
--EXPECT--
Hello
