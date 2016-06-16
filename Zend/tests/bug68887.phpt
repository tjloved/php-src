--TEST--
Bug #68887 (resources are not freed correctly)
--FILE--
<?php

function fn1664721512()
{
    fclose(fopen("php://temp", "w+"));
    $count = count(get_resources());
    fclose(fopen("php://temp", "w+"));
    var_dump(count(get_resources()) == $count);
    fclose(fopen("php://temp", "w+"));
    var_dump(count(get_resources()) == $count);
}
fn1664721512();
--EXPECT--
bool(true)
bool(true)
