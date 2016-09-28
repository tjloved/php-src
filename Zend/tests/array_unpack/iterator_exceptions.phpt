--TEST--
Iterator exceptions thrown during array unpacking
--FILE--
<?php

class ThrowingIterator implements Iterator {
    private $throwOn;
    public function __construct($throwOn) {
        $this->throwOn = $throwOn;
    }
    public function rewind() {
        if ($this->throwOn === __FUNCTION__) throw new Exception(__FUNCTION__);
    }
    public function valid() {
        if ($this->throwOn === __FUNCTION__) throw new Exception(__FUNCTION__);
        return true;
    }
    public function next() {
        if ($this->throwOn === __FUNCTION__) throw new Exception(__FUNCTION__);
    }
    public function current() {
        if ($this->throwOn === __FUNCTION__) throw new Exception(__FUNCTION__);
        return 'value';
    }
    public function key() {
        if ($this->throwOn === __FUNCTION__) throw new Exception(__FUNCTION__);
        return 'key';
    }
}

$methods = ['rewind', 'valid', 'next', 'current', 'key'];
$dummy = [$x = 42];
foreach ($methods as $method) {
    $it = new ThrowingIterator($method);
    try {
        $dummy + [new stdClass, ...$it, 42];
    } catch (Exception $e) {
        echo $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
rewind
valid
next
current
key
