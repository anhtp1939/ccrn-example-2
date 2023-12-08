import { NativeModules } from 'react-native';
const { CCRNModule } = NativeModules;
interface CCRNModuleInterface {
    startGame(gameId: string, data: string): void;
    dispatchEventToReact(eventName: string, eventData: string): void;
    dispatchEventToCocos(eventName: string, eventData: string): void;
}
export default CCRNModule as CCRNModuleInterface;