import coloredlogs, logging

class Logger:
    def __init__(self, module_name):
        coloredlogs.install(fmt='[%(asctime)-15s] %(name)s %(message)s')
        logging.basicConfig(level=logging.NOTSET)
        self.logger = logging.getLogger(module_name)

    def info(self, message):
        self.logger.info(message)

    def warning(self, message):
        self.logger.warning(message)

    def error(self, message):
        self.logger.error(message)
