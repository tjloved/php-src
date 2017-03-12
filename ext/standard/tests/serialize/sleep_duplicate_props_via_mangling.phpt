--TEST--
__sleep() returning duplicate properties via mangling
--FILE--
<?php

class Test {
    private $priv;

    public function __sleep() {
        return ["\0Test\0priv", "priv"];
    }
}

$s = serialize(new Test);
var_dump($s);

?>
--EXPECTF--
Notice: serialize(): "priv" is returned from __sleep multiple times in %s on line %d
string(35) "O:4:"Test":1:{s:10:" Test priv";N;}"
