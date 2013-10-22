--TEST--
CallbackFilterIterator 002
--FILE--
<?php

try {
	new CallbackFilterIterator();
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}

try {
	new CallbackFilterIterator(null);
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}

try {
	new CallbackFilterIterator(new ArrayIterator(array()), null);
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}

try {
	new CallbackFilterIterator(new ArrayIterator(array()), array());
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}

$it = new CallbackFilterIterator(new ArrayIterator(array(1)), function() {
	throw new Exception("some message");
});
try {
	foreach($it as $e);
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}

--EXPECT--
CallbackFilterIterator::__construct() expects exactly 2 parameters, 0 given
Argument 1 passed to CallbackFilterIterator::__construct() must implement interface Iterator, null given
CallbackFilterIterator::__construct() expects parameter 2 to be a valid callback, no array or string given
CallbackFilterIterator::__construct() expects parameter 2 to be a valid callback, array must have exactly two members
some message
