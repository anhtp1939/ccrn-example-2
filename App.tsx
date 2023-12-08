/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 *
 * @format
 */

import React from 'react';
import {Button, NativeEventEmitter, NativeModules, SafeAreaView} from 'react-native';
import CCRNModule from './ccrn/ts/CCRNModule';

const data = {
  walletId: "0x511bAd4Dcea098531Ae06D173580C4817b5BA503",
  signature: "0x82e97e9bfe7ceb6c4c51612e997f1573f2a872bcaea0614fa2a9e18c7574bdb71b80308a86a1152ef38b97131a264747b27456636a26d34ea2219d4243151f7c1c",
  host: "http://35.238.218.10:3001"
};

function App(): JSX.Element {
  return (
    <SafeAreaView>
      <Button
        title="happy-pop"
        onPress={() => startGame("happy-pop", JSON.stringify(data))}
      />
      <Button
        title="meteor-shooting"
        onPress={() => startGame("meteor-shooting", JSON.stringify(data))}
      />
      <Button
        title="ladder-run"
        onPress={() => startGame("ladder-run", JSON.stringify(data))}
      />
      <Button
        title="magic-touch"
        onPress={() => startGame("magic-touch", JSON.stringify(data))}
      />
    </SafeAreaView>
  );
}

function startGame(gameId: string, data:string) {
  CCRNModule.startGame(gameId, data);
};

const eventEmiiter = new NativeEventEmitter(NativeModules.CCRNModule);
eventEmiiter.addListener('EVENT_FROM_COCOS_TO_REACT', event => {
  console.log(`[React] EVENT_FROM_COCOS_TO_REACT - eventData = ${event}`);
});

export default App;
