import { Module } from '@nestjs/common';
import { AppController } from './app.controller';
import { Esp32Service } from './esp32.service';

@Module({
  imports: [],
  controllers: [AppController],
  providers: [Esp32Service],
})
export class AppModule {}
