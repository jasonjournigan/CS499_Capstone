from pymongo import MongoClient
from pymongo.errors import PyMongoError
import re
import logging

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

class CRUDOperations:
    """
    CRUD operations for MongoDB collections with connection pooling and enhanced security.
    """
    def __init__(self, username, password, host, port, db_name, collection_name, max_pool_size=50):
        """
        Initialize MongoDB client with connection pooling.

        :param username: MongoDB username
        :param password: MongoDB password
        :param host: MongoDB host
        :param port: MongoDB port
        :param db_name: Database name
        :param collection_name: Collection name
        :param max_pool_size: Maximum number of connections in the pool
        """
        try:
            self.client = MongoClient(
                f'mongodb://{username}:{password}@{host}:{port}',
                maxPoolSize=max_pool_size,
                connectTimeoutMS=5000,
                socketTimeoutMS=5000
            )
            self.database = self.client[db_name]
            self.collection = self.database[collection_name]
            logger.info(f"Connected to MongoDB at {host}:{port}")
        except PyMongoError as e:
            logger.error(f"Error initializing MongoDB client: {e}")
            raise Exception(f"Error initializing MongoDB client: {e}")

    def create_index(self, keys):
        """
        Create an index on the collection for performance optimization.

        :param keys: List of tuples specifying fields and direction (e.g., [('field', 1)])
        """
        try:
            self.collection.create_index(keys)
            logger.info(f"Created index on {keys}")
        except PyMongoError as e:
            logger.error(f"Error creating index: {e}")

    def validate_input(self, data):
        """
        Validate input data to prevent injection attacks.

        :param data: Dictionary to validate
        :return: True if valid, raises ValueError if invalid
        """
        for key, value in data.items():
            if isinstance(value, str):
                if re.search(r'[$<>{}\[\]]', value):
                    raise ValueError(f"Invalid characters in input: {value}")
        return True

    def create(self, data):
        """
        Inserts a document into the specified MongoDB collection.

        :param data: Dictionary representing the document to insert.
        :return: True if the insertion is successful, False otherwise.
        """
        if not isinstance(data, dict):
            raise ValueError("Data must be a dictionary.")
        try:
            self.validate_input(data)
            result = self.collection.insert_one(data)
            logger.info(f"Inserted document with ID: {result.inserted_id}")
            return result.acknowledged
        except PyMongoError as e:
            logger.error(f"Error inserting document: {e}")
            return False

    def read(self, query):
        """
        Queries documents from the specified MongoDB collection.

        :param query: Dictionary representing the query criteria.
        :return: List of documents matching the query, or an empty list if none match or an error occurs.
        """
        if not isinstance(query, dict):
            raise ValueError("Query must be a dictionary.")
        try:
            self.validate_input(query)
            cursor = self.collection.find(query)
            results = list(cursor)
            logger.info(f"Retrieved {len(results)} documents")
            return results
        except PyMongoError as e:
            logger.error(f"Error querying documents: {e}")
            return []

    def update(self, query, updates):
        """
        Update documents matching the query.

        :param query: Dictionary representing the query filter
        :param updates: Dictionary representing the updates to apply
        :return: Count of modified documents
        """
        if not isinstance(query, dict) or not isinstance(updates, dict):
            raise ValueError("Query and updates must be dictionaries.")
        try:
            self.validate_input(query)
            self.validate_input(updates)
            result = self.collection.update_many(query, updates)
            logger.info(f"Updated {result.modified_count} documents")
            return result.modified_count
        except PyMongoError as e:
            logger.error(f"Failed to update documents: {e}")
            return 0

    def delete(self, query):
        """
        Delete documents matching the query.

        :param query: Dictionary representing the query filter
        :return: Count of deleted documents
 """
        if not isinstance(query, dict):
            raise ValueError("Query must be a dictionary.")
        try:
            self.validate_input(query)
            result = self.collection.delete_many(query)
            logger.info(f"Deleted {result.deleted_count} documents")
            return result.deleted_count
        except PyMongoError as e:
            logger.error(f"Failed to delete documents: {e}")
            return 0

    def __del__(self):
        """
        Close MongoDB client connection when object is destroyed.
        """
        try:
            self.client.close()
            logger.info("MongoDB connection closed")
        except Exception as e:
            logger.error(f"Error closing MongoDB connection: {e}")