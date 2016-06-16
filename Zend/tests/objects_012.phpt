--TEST--
implementing a class
--FILE--
<?php

class foo
{
}
interface bar extends foo
{
}
function fn215599826()
{
    echo "Done\n";
}
fn215599826();
--EXPECTF--	
Fatal error: bar cannot implement foo - it is not an interface in %s on line %d
